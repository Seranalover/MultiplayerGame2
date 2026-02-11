// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_GroundBlast.generated.h"

/**
 * 地面爆破技能类
 */
UCLASS()
class UGA_GroundBlast : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_GroundBlast();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Aimation")
	UAnimMontage* GroundBlastMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TSubclassOf<class ATargetActor_GroundPick> TargetActorClass;
	
	UFUNCTION()
	void TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle); //确认选取目标
	
	UFUNCTION()
	void TargetCanceled(const FGameplayAbilityTargetDataHandle& TargetDataHandle); //取消选取目标
};
