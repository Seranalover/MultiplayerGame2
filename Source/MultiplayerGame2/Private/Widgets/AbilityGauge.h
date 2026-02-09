// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "AbilityGauge.generated.h"

/**
 * 技能图标UI
 */
UCLASS()
class UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	
private:
	UPROPERTY(meta=(BindWidget))
	class UImage* Icon;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownCounterText; //冷却计数
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownDurationText; //冷却持续时间
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CostText; //消耗
};
