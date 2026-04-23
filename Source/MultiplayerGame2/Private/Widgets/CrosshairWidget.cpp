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
		OwnerASC->GenericGameplayEventCallbacks.Add(UCAbilitySystemStatics::GetTargetUpdatedTag()).AddUObject(this, &UCrosshairWidget::TargetUpdated);
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
	//瞄准目标不存在准星居中
	if (!AimTarget)
	{
		FVector2D ViewportSize = FVector2D{(float)SizeX, (float)SizeY};
		CrosshairCanvasPanelSlot->SetPosition(ViewportSize / 2.f / ViewportScale);
		return;
	}
	//瞄准目标存在准星跟随瞄准目标，因为widget超出屏幕显示，系统会停止widget的刷新，导致widget无法恢复显示
	FVector2D TargetScreenPosition;
	CachedPlayerController->ProjectWorldLocationToScreen(AimTarget->GetActorLocation(), TargetScreenPosition);
	if (TargetScreenPosition.X > 0 && TargetScreenPosition.X < SizeX && TargetScreenPosition.Y > 0 && TargetScreenPosition.Y < SizeY)
		CrosshairCanvasPanelSlot->SetPosition(TargetScreenPosition / ViewportScale);
}

void UCrosshairWidget::TargetUpdated(const FGameplayEventData* EventData)
{
	AimTarget = EventData->Target;
	CrosshairImage->SetColorAndOpacity(AimTarget? HasTargetColor : NoTargetColor);
}
