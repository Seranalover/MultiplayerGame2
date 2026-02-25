// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_LevelBased.generated.h"

/**
 * 基于等级计算属性值类
 * 基于等级提升基础攻击、力量、智力等属性
 */
UCLASS()
class UMMC_LevelBased : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMMC_LevelBased();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute RateAttribute; //增长速率
	
	FGameplayEffectAttributeCaptureDefinition LevelCaptureDefinition; //捕获level属性
};
