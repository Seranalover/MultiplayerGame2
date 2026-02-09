// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityGauge.h"

#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/CAbilitySystemStatics.h"

void UAbilityGauge::NativeConstruct()
{
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
}

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);
	float CooldownDuration = UCAbilitySystemStatics::GetStaticCooldownDurationForAbility(AbilityCDO); //获得技能冷却
	float Cost = UCAbilitySystemStatics::GetStaticCostForAbility(AbilityCDO); //获得技能消耗
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration)); //设置技能冷却值文本
	CostText->SetText(FText::AsNumber(Cost)); //设置技能消耗值文本
}

void UAbilityGauge::ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData)
{
	if (Icon && WidgetData)
	{
		Icon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, WidgetData->Icon.LoadSynchronous());
	}
}
