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
	UPROPERTY(EditDefaultsOnly, Category="Launch")
	TSubclassOf<UGameplayEffect> GameplayEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* UpperCutMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Targetting")
	float TargetSweepSphereRadius = 80.f; //碰撞扫描半径
	
	static FGameplayTag GetUpperCutLaunchTag(); //上勾拳开始tag，同时接收target信息
	
	UPROPERTY(EditDefaultsOnly, Category="Launch")
	float UpperLaunchVelocity = 1000.f; //浮空速度
	
	UFUNCTION()
	void StartLaunching(FGameplayEventData EventData);
};
