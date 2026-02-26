// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CGameplayAbilityTypes.h"
#include "CAbilitySystemComponent.generated.h"

/**
 * GAS组件
 */
UCLASS()
class UCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	UCAbilitySystemComponent();
	void ServerSideInit();
	void InitializeBaseAttributes();
	void ApplyFullStatEffect(); //回满状态
	const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const; //获得附加技能，不包括基础技能
	bool IsAtMaxLevel() const; //是否满级
	
	UFUNCTION(Server, Reliable, WithValidation) //server only
	void Server_UpgradeAbilityWithInputID(ECAbilityInputID InputID); //server使用技能点升级技能
	UFUNCTION(Client, Reliable) //client only
	void Client_AbilitySpecLevelUpdated(FGameplayAbilitySpecHandle Handle, int NewLevel); //通知client技能已升级
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities; //附加技能
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities; //基础技能
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	class UPA_AbilitySystemGenerics* AbilitySystemGenerics;
	
	void ApplyInitialEffects(); //初始化所有数值
	void GiveInitialAbilities(); //赋予技能
	void HealthUpdated(const FOnAttributeChangeData& ChangeData); //生命值变更
	void ManaUpdated(const FOnAttributeChangeData& ChangeData); //魔力值变更
	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);
	void ExperienceUpdated(const FOnAttributeChangeData& ChangeData); //经验值变更
};
