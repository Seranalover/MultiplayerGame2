// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_AbilitySystemGenerics.generated.h"

class UGameplayEffect;
class UGameplayAbility;
/**
 * GAS通用组件提取类
 */
UCLASS()
class UPA_AbilitySystemGenerics : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDeathEffect() const { return DeathEffect; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetFullStatEffect() const { return FullStatEffect; }
	FORCEINLINE const TArray<TSubclassOf<UGameplayEffect>>& GetInitialEffects() const { return InitialEffects; }
	FORCEINLINE const TArray<TSubclassOf<UGameplayAbility>>& GetPassiveAbilities() const { return PassiveAbilities; }
	FORCEINLINE const UDataTable* GetBaseStatsDataTable() const { return BaseStatsDataTable; }
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TSubclassOf<UGameplayEffect> DeathEffect; //死亡GE
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TSubclassOf<UGameplayEffect> FullStatEffect; //复活GE
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities; //被动技能
	
	UPROPERTY(EditDefaultsOnly, Category="Base Stats")
	UDataTable* BaseStatsDataTable;
};
