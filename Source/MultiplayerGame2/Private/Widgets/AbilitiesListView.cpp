// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilitiesListView.h"
#include "Abilities/GameplayAbility.h"

void UAbilitiesListView::ConfigureAbilities(
	const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities)
{
	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		AddItem(AbilityPair.Value.GetDefaultObject());
	}
}
