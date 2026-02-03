// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CAbilitySystemStatics.generated.h"

/**
 * 静态tag类
 */
UCLASS()
class UCAbilitySystemStatics : public UObject
{
	GENERATED_BODY()
public:
	static FGameplayTag GetBasicAttackAbilityTag(); //基础攻击tag
	static FGameplayTag GetDeadStatTag(); //死亡状态tag
	
};
