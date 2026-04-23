// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CrosshairWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "GAS/CAbilitySystemStatics.h"

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetCrosshairTag()).AddUObject(this, &UCrosshairWidget::CrosshairTagUpdated);
	}
	CachedPlayerController = GetOwningPlayer();
	CrosshairCanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
	if (!CrosshairCanvasPanelSlot)
	{
		UE_LOG(LogTemp, Error, TEXT("Crosshair widget need to be parented under a canvas panel to place itself properly"));
	}
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (CrosshairImage->GetVisibility() == ESlateVisibility::Visible)
	{
		UpdateCrosshairPosition();	
	}
}

void UCrosshairWidget::CrosshairTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	CrosshairImage->SetVisibility(NewCount > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UCrosshairWidget::UpdateCrosshairPosition()
{
	if (!CachedPlayerController ||!CrosshairCanvasPanelSlot) return;
	
	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this); //视口缩放系数
	int32 SizeX, SizeY;
	CachedPlayerController->GetViewportSize(SizeX, SizeY);
	FVector2D ViewportSize = FVector2D{(float)SizeX, (float)SizeY};
	CrosshairCanvasPanelSlot->SetPosition(ViewportSize / 2.f / ViewportScale);
}
