// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "CAbilitySystemStatics.generated.h"

class UGameplayAbility;
struct FGameplayAbilitySpec;
class UAbilitySystemComponent;

/**
 * 静态资源类
 */
UCLASS()
class UCAbilitySystemStatics : public UObject
{
	GENERATED_BODY()
public:
	static FGameplayTag GetBasicAttackAbilityTag(); //基础攻击tag
	static FGameplayTag GetDeadStatTag(); //死亡状态tag
	static FGameplayTag GetStunStatTag(); //眩晕tag
	static FGameplayTag GetAimStatTag(); //瞄准状态tag
	static FGameplayTag GetFocusStatTag(); //专注状态tag
	static FGameplayTag GetHealthFullStatTag();
	static FGameplayTag GetHealthEmptyStatTag();
	static FGameplayTag GetManaFullStatTag();
	static FGameplayTag GetManaEmptyStatTag();
	static FGameplayTag GetHeroRoleTag(); //英雄角色标签
	static FGameplayTag GetExperienceAttributeTag();
	static FGameplayTag GetGoldAttributeTag();
	static FGameplayTag GetGenericDamagePointTag();
	/*
	 * 按下攻击键tag
	 * 用于按下攻击键时，发送信号触发其他事件
	 * usage：上勾拳时按下攻击键，可以附加其他连招
	 */
	static FGameplayTag GetBasicAttackInputPressedTag();
	static FGameplayTag GetBasicAttackInputReleasedTag(); //释放攻击键
	static FGameplayTag GetCameraShakeGameplayCueTag(); //获得相机震动的GC tag，用于调用GC时使用
	static FGameplayTag GetCrosshairTag(); //带有tag时显示准星
	static FGameplayTag GetTargetUpdatedTag(); //瞄准目标变更tag
	
	static float GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability); //查询技能冷却时间
	static float GetStaticCostForAbility(const UGameplayAbility* Ability); //查询技能消耗
	static bool IsHero(const AActor* Actor); //查询是否英雄单位
	static bool ActorHasTag(const AActor* ActorToCheck, const FGameplayTag& Tag); //actor携带指定标签？
	static bool IsActorDead(const AActor* ActorToCheck); //actor死亡？
	static bool IsAbilityAtMaxLevel(const FGameplayAbilitySpec& AbilitySpec); //技能等级已满？
	static bool CheckAbilityCost(const FGameplayAbilitySpec& AbilitySpec, const UAbilitySystemComponent& ASC); //检查技能是否满足消耗
	static bool CheckAbilityCostStatic(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC); //检查技能是否满足消耗
	static float GetManaCostFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel); //查询mana消耗
	static float GetCooldownDurationFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel); //查询冷却时间
	static float GetCooldownRemainingFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC); //查询冷却剩余时间
};
