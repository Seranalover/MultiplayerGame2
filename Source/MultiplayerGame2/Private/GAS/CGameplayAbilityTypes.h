// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CGameplayAbilityTypes.generated.h"

/**
 * Ability InputID枚举类
 */
UENUM(BlueprintType)
enum class ECAbilityInputID : uint8
{
	None								UMETA(DisplayName = "None"),
	Confirm								UMETA(DisplayName = "Confirm"),
	Cancel								UMETA(DisplayName = "Cancel"),
	BasicAttack							UMETA(DisplayName = "Basic Attack"),
	AbilityOne							UMETA(DisplayName = "Ability One"),
	AbilityTwo							UMETA(DisplayName = "Ability Two"),
	AbilityThree						UMETA(DisplayName = "Ability Three"),
	AbilityFour							UMETA(DisplayName = "Ability Four"),
	AbilityFive							UMETA(DisplayName = "Ability Five"),
	AbilitySix							UMETA(DisplayName = "Ability Six"),
	
};

/**
 * 伤害效果结构体
 */
USTRUCT(BlueprintType)
struct FGenericDamageEffectDef
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect; //伤害效果
	
	UPROPERTY(EditAnywhere)
	FVector PushVelocity = FVector::ZeroVector; //推动方向
};

/**
 * 英雄基础属性结构体
 */
USTRUCT()
struct FHeroBaseStats : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;
	UPROPERTY(EditAnywhere)
	float Strength = 0.0f;
	UPROPERTY(EditAnywhere)
	float Intelligence = 0.0f;
	UPROPERTY(EditAnywhere)
	float StrengthGrowthRate = 0.0f; //力量成长值
	UPROPERTY(EditAnywhere)
	float IntelligenceGrowthRate = 0.0f; //智力成长值
	UPROPERTY(EditAnywhere)
	float BaseMaxHealth = 0.0f;
	UPROPERTY(EditAnywhere)
	float BaseMaxMana = 0.0f;
	UPROPERTY(EditAnywhere)
	float BaseAttackDamage = 0.0f;
	UPROPERTY(EditAnywhere)
	float BaseArmor = 0.0f;
	UPROPERTY(EditAnywhere)
	float BaseMoveSpeed = 0.0f;
};
