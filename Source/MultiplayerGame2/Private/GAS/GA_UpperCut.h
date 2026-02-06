// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_UpperCut.generated.h"

/**
 * 技能-上勾拳
 */
UCLASS()
class UGA_UpperCut : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* UpperCutMontage;
};
