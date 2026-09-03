// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbilityTypes.h"
#include "GAS/CGameplayAbility.h"
#include "GA_Freeze.generated.h"

/**
 * 技能 - 冰冻
 */
UCLASS()
class UGA_Freeze : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Freeze();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Aimation")
	UAnimMontage* TargetMontage; //瞄准动画
	
	UPROPERTY(EditDefaultsOnly, Category="Aimation")
	UAnimMontage* CastMontage; //施放技能动画
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TSubclassOf<class ATargetActor_GroundPick> TargetActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetAreaRadius = 300.f; //目标范围
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetTraceRange = 2000.f; //射程
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGenericDamageEffectDef DamageEffectDef; //伤害效果
	
	UPROPERTY(EditDefaultsOnly, Category="Cue")
	FGameplayTag FreezeCueTag;
	
	UFUNCTION()
	void TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle); //确认选取目标
	
	UFUNCTION()
	void TargetCanceled(const FGameplayAbilityTargetDataHandle& TargetDataHandle); //取消选取目标
};
