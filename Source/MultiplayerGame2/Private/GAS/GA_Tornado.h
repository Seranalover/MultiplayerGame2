// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_Tornado.generated.h"

/**
 * 技能 - 龙卷风
 */
UCLASS()
class UGA_Tornado : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	UAnimMontage* TornadoMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TornadoDuration = 6.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effect")
	TSubclassOf<UGameplayEffect> HitDamageEffect;
	
	UFUNCTION()
	void TornadoDamageEventReceived(FGameplayEventData Payload);
};
