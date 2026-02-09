// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayWidget.h"

#include "AbilitiesListView.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ValueGauge.h"
#include "GAS/CAbilitySystemComponent.h"

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn()); //从player pawn获取GAS组件
	if (OwnerAbilitySystemComponent)
	{
		HealthBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
		ManaBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());
	}
	// 耦合度过高，依赖于ASC组件，已在Ccharacter类中重新实现
	// const UCAbilitySystemComponent* CAbilitySystemComponent = Cast<UCAbilitySystemComponent>(OwnerAbilitySystemComponent);
	// if (CAbilitySystemComponent)
	// {
	// 	ConfigureAbilities(CAbilitySystemComponent->GetAbilities());
	// }

}

void UGameplayWidget::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities)
{
	AbilitiesListView->ConfigureAbilities(Abilities);
}
