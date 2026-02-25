// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/MMC_LevelBased.h"

#include "CHeroAttributeSet.h"

UMMC_LevelBased::UMMC_LevelBased()
{
	LevelCaptureDefinition.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	LevelCaptureDefinition.AttributeToCapture = UCHeroAttributeSet::GetLevelAttribute();
	RelevantAttributesToCapture.Add(LevelCaptureDefinition);
}

float UMMC_LevelBased::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	UAbilitySystemComponent* ASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();
	if (!ASC) return 0.0f;
	
	float Level = 0;
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	GetCapturedAttributeMagnitude(LevelCaptureDefinition, Spec, EvaluateParameters, Level);
	
	bool bFound;
	float RateAttributeValue = ASC->GetGameplayAttributeValue(RateAttribute, bFound);
	if (!bFound) return 0.0f;
	
	return (Level - 1) * RateAttributeValue;
}
