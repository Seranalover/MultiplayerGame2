// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_PassiveLaunched.generated.h"

/**
 * 浮空技能-被动触发
 */
UCLASS()
class UGA_PassiveLaunched : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_PassiveLaunched();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	static FGameplayTag GetLaunchedAbilityTriggerTag(); //获得激活技能的tag
};
