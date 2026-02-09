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
	
private:
	UPROPERTY(EditAnywhere, Category="Data")
	UDataTable* AbilityDataTable;
	
	//在data table中查找ability信息
	const struct FAbilityWidgetData* FindWidgetDataForAbility(const TSubclassOf<UGameplayAbility>& AbilityClass) const;
	
	void AbilityGaugeGenerated(UUserWidget& Widget);
};
