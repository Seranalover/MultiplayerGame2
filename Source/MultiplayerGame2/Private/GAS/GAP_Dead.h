// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GAP_Dead.generated.h"

/**
 * Pass dead event
 * 死亡事件传递技能
 * 用于单位死亡时，给予击杀者和盟友金钱和经验值
 */
UCLASS()
class UGAP_Dead : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGAP_Dead();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
