// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CAttributeSet.generated.h"

//宏，可以自动生成属性的get和set方法
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 属性集
 */
UCLASS()
class UCAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	ATTRIBUTE_ACCESSORS(UCAttributeSet, Health); //调用父类的宏，生成get和set方法
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UCAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxMana);
	
private:
	UPROPERTY()
	FGameplayAttributeData Health;
	UPROPERTY()
	FGameplayAttributeData MaxHealth;
	UPROPERTY()
	FGameplayAttributeData Mana;
	UPROPERTY()
	FGameplayAttributeData MaxMana;
};
