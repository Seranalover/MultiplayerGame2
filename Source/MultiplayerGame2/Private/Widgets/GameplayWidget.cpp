// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayWidget.h"

#include "AbilitiesListView.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ShopWidget.h"
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

void UGameplayWidget::ToggleShop()
{
	if (ShopWidget->GetVisibility() == ESlateVisibility::HitTestInvisible)
	{
		ShopWidget->SetVisibility(ESlateVisibility::Visible); //显示商店页面
		PlayShopPopupAnimation(true); //播放打开商店动画
		SetOwningPawnInputEnabled(false); //禁用pawn输入
		SetShowMouseCursor(true); //显示鼠标
		SetFocusToGameAndUI(); //设置焦点为游戏和UI
		ShopWidget->SetFocus(); //设置焦点为商店页面
	}
	else
	{
		ShopWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		PlayShopPopupAnimation(false);
		SetOwningPawnInputEnabled(true);
		SetShowMouseCursor(false);
		SetFocusToGameOnly();
	}
}

void UGameplayWidget::PlayShopPopupAnimation(bool bPlayForward)
{
	if (bPlayForward)
		PlayAnimationForward(ShopPopupAnimation);
	else
		PlayAnimationReverse(ShopPopupAnimation);
}

void UGameplayWidget::SetOwningPawnInputEnabled(bool bEnable)
{
	if (bEnable)
	{
		GetOwningPlayerPawn()->EnableInput(GetOwningPlayer());
	}
	else
	{
		GetOwningPlayerPawn()->DisableInput(GetOwningPlayer());
	}
}

void UGameplayWidget::SetShowMouseCursor(bool bShow)
{
	GetOwningPlayer()->SetShowMouseCursor(bShow);
}

void UGameplayWidget::SetFocusToGameAndUI()
{
	FInputModeGameAndUI InputModeGameAndUI;
	InputModeGameAndUI.SetHideCursorDuringCapture(false);
	GetOwningPlayer()->SetInputMode(InputModeGameAndUI);
}

void UGameplayWidget::SetFocusToGameOnly()
{
	FInputModeGameOnly InputModeGameOnly;
	GetOwningPlayer()->SetInputMode(InputModeGameOnly);
}
