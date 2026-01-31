// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadStatsGauge.generated.h"

/**
 * 
 */
UCLASS()
class UOverheadStatsGauge : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ConfigureWithASC(class UAbilitySystemComponent* AbilitySystemComponent); //利用ASC组件设置属性值
	
private:
	UPROPERTY(meta=(BindWidget))
	class UValueGauge* HealthBar;
	
	UPROPERTY(meta=(BindWidget))
	class UValueGauge* ManaBar;
	
};
