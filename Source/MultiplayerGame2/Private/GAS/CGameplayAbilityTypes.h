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
	Aim									UMETA(DisplayName = "Aim"),
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
	FGenericDamageEffectDef();
	
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
	
public:
	FHeroBaseStats();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;
	UPROPERTY(EditAnywhere)
	float Strength;
	UPROPERTY(EditAnywhere)
	float Intelligence;
	UPROPERTY(EditAnywhere)
	float StrengthGrowthRate; //力量成长值
	UPROPERTY(EditAnywhere)
	float IntelligenceGrowthRate; //智力成长值
	UPROPERTY(EditAnywhere)
	float BaseMaxHealth;
	UPROPERTY(EditAnywhere)
	float BaseMaxMana;
	UPROPERTY(EditAnywhere)
	float BaseAttackDamage;
	UPROPERTY(EditAnywhere)
	float BaseArmor;
	UPROPERTY(EditAnywhere)
	float BaseMoveSpeed;
};
