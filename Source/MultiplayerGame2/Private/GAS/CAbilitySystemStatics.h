// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "CAbilitySystemStatics.generated.h"

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
	static FGameplayTag GetHealthFullStatTag();
	static FGameplayTag GetHealthEmptyStatTag();
	static FGameplayTag GetManaFullStatTag();
	static FGameplayTag GetManaEmptyStatTag();
	static FGameplayTag GetHeroRoleTag(); //英雄角色标签
	static FGameplayTag GetExperienceAttributeTag();
	static FGameplayTag GetGoldAttributeTag();
	
	/*
	 * 按下攻击键tag
	 * 用于按下攻击键时，发送信号触发其他事件
	 * usage：上勾拳时按下攻击键，可以附加其他连招
	 */
	static FGameplayTag GetBasicAttackInputPressedTag();
	//获得相机震动的GC tag，用于调用GC时使用
	static FGameplayTag GetCameraShakeGameplayCueTag(); 
	
	static float GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability); //查询技能冷却时间
	static float GetStaticCostForAbility(const UGameplayAbility* Ability); //查询技能消耗
	static bool IsHero(const AActor* Actor); //查询是否英雄单位

};
