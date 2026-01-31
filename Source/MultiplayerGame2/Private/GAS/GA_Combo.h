// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_Combo.generated.h"

/**
 * 连击类
 */
UCLASS()
class UGA_Combo : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Combo();
	//重要函数ActivateAbility，激活并执行技能
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

private:
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* ComboMontage;
	
	
};
