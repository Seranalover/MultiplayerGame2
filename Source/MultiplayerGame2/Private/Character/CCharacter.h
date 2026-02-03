// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "CCharacter.generated.h"

/**
 * 角色基类
 */
UCLASS()
class ACCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACCharacter();
	void ServerSideInit(); //服务端初始化
	void ClientSideInit(); //客户端初始化
	bool IsLocallyControlledByPlayer() const; //是否由本地玩家控制器操控
	virtual void PossessedBy(AController* NewController) override; //only called on the Server

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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
private:
	UPROPERTY(VisibleDefaultsOnly, Category="Gameplay Ability")
	class UCAbilitySystemComponent* CAbilitySystemComponent;
	UPROPERTY(VisibleDefaultsOnly, Category="Gameplay Ability")
	class UCAttributeSet* CAttributeSet;
	
	void BindGASChangeDelegates(); //绑定所有从GAS监听到的变更
	
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
	/*                             Death and Respawn                            */
	/****************************************************************************/
private:
	void DeathTagUpdated(const FGameplayTag Tag, int32 NewCount); //死亡tag变更
	void StartDeathSequence(); //开始执行死亡任务序列
	void Respawn(); //重生
};
