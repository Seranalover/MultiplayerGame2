// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_BaseAttackDamage.generated.h"

/**
 * 伤害修改计算类
 */
UCLASS()
class UMMC_BaseAttackDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMMC_BaseAttackDamage();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition DamageCaptureDef; //捕获damage属性
	FGameplayEffectAttributeCaptureDefinition ArmorCaptureDef; //捕获armor属性
	
};
