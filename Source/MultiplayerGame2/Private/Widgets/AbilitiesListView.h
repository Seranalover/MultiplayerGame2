// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "AbilitiesListView.generated.h"

/**
 * 技能列表控件
 */
UCLASS()
class UAbilitiesListView : public UListView
{
	GENERATED_BODY()
	
public:
	void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities);
};
