// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_BlackHole.generated.h"

/**
 * 技能 - 黑洞
 */
UCLASS()
class UGA_BlackHole : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetAreaRadius = 1000.f; //目标范围
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float BlackHolePullSpeed = 3000.f; //牵引敌人的速度
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetTraceRange = 2000.f; //射程
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float BlackHoleDuration = 6.f; //持续时间
	
	UPROPERTY(EditDefaultsOnly, Category="Aimation")
	UAnimMontage* TargetingMontage; //瞄准动画
	
	UPROPERTY(EditDefaultsOnly, Category="Aimation")
	UAnimMontage* HoldBlackHoleMontage; //施放动画
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TSubclassOf<UGameplayEffect> AimEffect;
	
	FActiveGameplayEffectHandle AimEffectHandle;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TSubclassOf<class ATargetActor_GroundPick> TargetActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TSubclassOf<class ATA_BlackHole> BlackHoleTargetActorClass;
	
	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* PlayCastBlackHoleMontageTask;
	
	UPROPERTY()
	class UAbilityTask_WaitTargetData* BlackHoleTargetingTask;
	
	UFUNCTION()
	void PlaceBlackHole(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	UFUNCTION()
	void PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle); 
	UFUNCTION()
	void FinalTargetsReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	
	void AddAimEffect();
	void RemoveAimEffect();
};
