// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

/**
 * 十字准星
 */
UCLASS()
class UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="View")
	FLinearColor HasTargetColor = FLinearColor::Red;
	
	UPROPERTY(EditDefaultsOnly, Category="View")
	FLinearColor NoTargetColor = FLinearColor::White;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* CrosshairImage;
	
	UPROPERTY()
	class UCanvasPanelSlot* CrosshairCanvasPanelSlot;
	
	UPROPERTY()
	class APlayerController* CachedPlayerController;
	
	UPROPERTY()
	const AActor* AimTarget;
	
	void CrosshairTagUpdated(const FGameplayTag Tag, int32 NewCount);
	void UpdateCrosshairPosition();
	void TargetUpdated(const struct FGameplayEventData* EventData);
};
