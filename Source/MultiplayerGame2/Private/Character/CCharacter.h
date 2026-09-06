// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameFramework/Character.h"
#include "Widgets/RenderActorTargetInterface.h"
#include "CCharacter.generated.h"

enum class ECAbilityInputID : uint8;
/**
 * 角色基类
 */
UCLASS()
class ACCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface, public IRenderActorTargetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACCharacter();
	void ServerSideInit(); //服务端初始化
	void ClientSideInit(); //客户端初始化
	bool IsLocallyControlledByPlayer() const; //是否由本地玩家控制器操控
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override; //管理数据生命周期并同步到客户端
	const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const; //获得附加技能，不包括基础技能
	virtual FVector GetCaptureLocalPosition() const override; //捕捉相对位置
	virtual FRotator GetCaptureLocalRotation() const override; //捕捉相对旋转
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Capture")
	FVector HeadshotCaptureLocalPosition;
	
	UPROPERTY(EditDefaultsOnly, Category="Capture")
	FRotator HeadshotCaptureLocalRotation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override; //only called on the Server

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/****************************************************************************/
	/*                            Gameplay Ability                              */
	/****************************************************************************/
public:	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFUNCTION(Server, Reliable, WithValidation) //client调用该函数时，会通知server调用相同的函数
	void Server_SendGameplayEventToSelf(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
	FORCEINLINE bool GetIsInFocusMode() const { return bIsInFocusMode; }
	
protected:
	void UpgradeAbilityWithInputID(ECAbilityInputID InputID);
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category="Gameplay Ability")
	class UCAbilitySystemComponent* CAbilitySystemComponent;
	UPROPERTY(VisibleDefaultsOnly, Category="Gameplay Ability")
	class UCAttributeSet* CAttributeSet;
	
	bool bIsInFocusMode = false;
	
	void BindGASChangeDelegates(); //绑定所有从GAS监听到的变更
	void DeathTagUpdated(const FGameplayTag Tag, int32 NewCount); //死亡tag变更
	void StunTagUpdated(const FGameplayTag Tag, int32 NewCount); //stun tag变更
	void AimTagUpdated(const FGameplayTag Tag, int32 NewCount); //aim tag变更
	void FocusTagUpdated(const FGameplayTag Tag, int32 NewCount); //focus tag变更
	void SetIsAiming(bool bIsAiming); //瞄准时的处理
	virtual void OnAimStateChanged(bool bIsAiming);
	void MoveSpeedUpdated(const FOnAttributeChangeData& Data);
	void MoveAccelerationUpdated(const FOnAttributeChangeData& Data);
	void MaxHealthUpdated(const FOnAttributeChangeData& Data);
	void MaxManaUpdated(const FOnAttributeChangeData& Data);
	
	/****************************************************************************/
	/*                                    UI                                    */
	/****************************************************************************/
private:
	UPROPERTY(VisibleDefaultsOnly, Category="UI")
	class UWidgetComponent* OverheadWidgetComponent;
	void ConfigureOverheadWidget(); //设置属性值
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	float OverheadWidgetVisibilityCheckGap = 1.f; //检测时间间隔，计时器执行周期
	UPROPERTY(EditDefaultsOnly, Category="UI")
	float OverheadWidgetVisibilityRangeSquared = 10000000.f; //检测距离的平方
	FTimerHandle OverheadWidgetVisibilityTimerHandle; //头顶状态条可视计时器
	void UpdateOverheadWidgetVisibility(); //设置可视性，被计时器周期性调用
	
	/****************************************************************************/
	/*                                 Stun                                     */
	/****************************************************************************/
private:
	UPROPERTY(editDefaultsOnly, Category="Stun")
	UAnimMontage* StunAnimMontage;
	
	virtual void OnStun();
	virtual void OnRecoverFromStun();
	
	/****************************************************************************/
	/*                             Death and Respawn                            */
	/****************************************************************************/
public:
	bool IsDead() const;
	void RespawnImmediately(); //立即复活
private:
	UPROPERTY(EditDefaultsOnly, Category="Death")
	UAnimMontage* DeathAnimMontage;
	UPROPERTY(EditDefaultsOnly, Category="Death")
	float DeathMontageFinishTimeShift = -0.8f; //动画播放时间偏移，用于在死亡动画中途就开启布偶效果
	FTimerHandle DeathMontageTimerHandle; //死亡动画计时器
	FTransform MeshRelativeTransform; //骨骼相对位置
	
	void StartDeathSequence(); //开始执行死亡任务序列
	void Respawn(); //重生
	void PlayDeathAnimation(); //播放死亡动画
	void SetStatsGaugeEnabled(bool bIsEnable); //显示血条
	virtual void OnDead();
	virtual void OnRespawn();
	void DeathMontageFinished();
	void SetRagdollEnabled(bool bIsEnable); //设置布偶效果
	
	/****************************************************************************/
	/*                                   Team                                   */
	/****************************************************************************/
public:
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override; //设置team id
	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override; //获得team id
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamID)
	FGenericTeamId TeamId;
	UFUNCTION()
	virtual void OnRep_TeamID(); //SetGenericTeamId()被调用时，TeamId在服务端还没被复制，因此手动复制
	
	/****************************************************************************/
	/*                                    AI                                    */
	/****************************************************************************/
private:
	UPROPERTY()
	class UAIPerceptionStimuliSourceComponent* PerceptionStimuliSourceComponent; //感知刺激源组件
	
	void SetAIPerceptionStimuliSourceEnabled(bool bIsEnable); //设置是否开启感知刺激源
};
