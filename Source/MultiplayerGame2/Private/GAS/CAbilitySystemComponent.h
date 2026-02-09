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
	void ApplyInitialEffects(); //初始化所有数值
	void GiveInitialAbilities(); //赋予技能
	void ApplyFullStatEffect(); //回满状态
	const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const; //获得附加技能，不包括基础技能
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities; //附加技能
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities; //基础技能
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TSubclassOf<UGameplayEffect> DeathEffect; //死亡GE
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TSubclassOf<UGameplayEffect> FullStatEffect; //复活GE
	
	void HealthUpdated(const FOnAttributeChangeData& ChangeData); //生命值变更
	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);
};
