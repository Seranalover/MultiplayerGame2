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
	/*
	 * 按下攻击键tag
	 * 用于按下攻击键时，发送信号触发其他事件
	 * usage：上勾拳时按下攻击键，可以附加其他连招
	 */
	static FGameplayTag GetBasicAttackInputPressedTag(); 
	static FGameplayTag GetDeadStatTag(); //死亡状态tag
	static FGameplayTag GetStunStatTag(); 
	
};
