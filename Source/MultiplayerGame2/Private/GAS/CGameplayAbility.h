// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "Abilities/GameplayAbility.h"
#include "CGameplayAbility.generated.h"

/**
 * 能力基础类
 */
UCLASS()
class UCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UCGameplayAbility();
	/** Returns true if this ability can be activated right now. Has no side effects */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
protected:
	class UAnimInstance* GetOwnerAnimInstance() const; //动画实例
	
	TArray<FHitResult> GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, 
		float SphereSweepRadius = 30.0f, ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, bool bDrawDebug = false, 
		bool bIgnoreSelf = true) const; //Target插槽扫描结果
	
	FORCEINLINE bool ShouldDrawDebug() const { return bShouldDrawDebug; } //是否开启绘制debug
	
	void PushSelf(const FVector& PushVelocity); //将自身推向某个方向，作用于浮空、被击飞等场景
	
	void PushTarget(AActor* Target, const FVector& PushVelocity); //将目标推向某个方向
	
	void PushTargets(const TArray<AActor*>& Targets, const FVector& PushVelocity); //将多个目标推向某个方向
	void PushTargets(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVelocity); //将多个目标推向某个方向
	
	ACharacter* GetOwningAvatarCharacter();
	
	void ApplyGameplayEffectToHitResult(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1); //对目标应用攻击效果
	
	void PlayMontageLocally(UAnimMontage* MontageToPlay); //client同步播放server的动画
	void StopMontageAfterCurrentSection(UAnimMontage* MontageToStop); //当前动画片段播放后停止播放
	
	FGenericTeamId GetOwnerTeamId() const;
	
	AActor* GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const; //获得瞄准目标
	bool IsActorTeamAttitude(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const; //瞄准目标是否由team attitude管理
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bShouldDrawDebug = false;
	
	UPROPERTY()
	class ACharacter* CharacterRef;
};
