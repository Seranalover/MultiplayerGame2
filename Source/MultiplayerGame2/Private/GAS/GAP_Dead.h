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

private:
	UPROPERTY(EditDefaultsOnly, Category="Reward")
	float RewardRange = 1000.f; //击杀奖励范围
	
	UPROPERTY(EditDefaultsOnly, Category="Reward")
	float BaseExperienceReward = 200.f; //基础经验奖励
	
	UPROPERTY(EditDefaultsOnly, Category="Reward")
	float BaseGoldReward = 200.f; //基础金币奖励
	
	UPROPERTY(EditDefaultsOnly, Category="Reward")
	float ExperienceRewardPerExperience = 200.f; //额外经验奖励
	
	UPROPERTY(EditDefaultsOnly, Category="Reward")
	float GoldRewardPerExperience = 200.f; //额外金币奖励
	
	float KillerRewardPortion = 0.5f; //Killer奖励占比
	
	UPROPERTY(EditDefaultsOnly, Category="Reward")
	TSubclassOf<UGameplayEffect> RewardEffect; //奖励GE
	
	TArray<AActor*> GetRewardActors() const;
};
