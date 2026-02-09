// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilitiesListView.h"

#include "AbilityGauge.h"
#include "Abilities/GameplayAbility.h"

void UAbilitiesListView::ConfigureAbilities(
	const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities)
{
	OnEntryWidgetGenerated().AddUObject(this, &UAbilitiesListView::AbilityGaugeGenerated);
	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		AddItem(AbilityPair.Value.GetDefaultObject());
	}
}

void UAbilitiesListView::AbilityGaugeGenerated(UUserWidget& Widget)
{
	UAbilityGauge* AbilityGauge = Cast<UAbilityGauge>(&Widget);
	
	if (AbilityGauge)
	{
		AbilityGauge->ConfigureWithWidgetData(FindWidgetDataForAbility(AbilityGauge->GetListItem<UGameplayAbility>()->GetClass()));
	}
}

const struct FAbilityWidgetData* UAbilitiesListView::FindWidgetDataForAbility(
	const TSubclassOf<UGameplayAbility>& AbilityClass) const
{
	if (!AbilityDataTable) return nullptr;
	
	for (auto& AbilityWidgetDataPair : AbilityDataTable->GetRowMap())
	{
		const FAbilityWidgetData* WidgetData = AbilityDataTable->FindRow<FAbilityWidgetData>(AbilityWidgetDataPair.Key, "");
		if (WidgetData->AbilityClass ==  AbilityClass)
			return WidgetData;
	}
	return nullptr;
}
