// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CAbilitySystemComponent.generated.h"

/**
 * GAS组件
 */
UCLASS()
class UCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void ApplyInitialEffects(); //初始化所有数值
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
};
