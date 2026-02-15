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
	ATTRIBUTE_ACCESSORS(UCAttributeSet, AttackDamage);
	ATTRIBUTE_ACCESSORS(UCAttributeSet, Armor);
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MoveSpeed);
	//指定向客户端复制属性的方式
	virtual void GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const override;
	
	/**
		 *	Called just before any modification happens to an attribute. This is lower level than PreAttributeModify/PostAttribute modify.
		 *	There is no additional context provided here since anything can trigger this. Executed effects, duration based effects, effects being removed, immunity being applied, stacking rules changing, etc.
		 *	This function is meant to enforce things like "Health = Clamp(Health, 0, MaxHealth)" and NOT things like "trigger this extra thing if damage is applied, etc".
		 *	
		 *	NewValue is a mutable reference so you are able to clamp the newly applied value as well.
		 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override; //限制属性的最大值和最小值
	
	/**
	 *	Called just after a GameplayEffect is executed to modify the base value of an attribute. No more changes can be made.
	 *	Note this is only called during an 'execute'. E.g., a modification to the 'base value' of an attribute. It is not called during an application of a GameplayEffect, such as a 5 second +10 movement speed buff.
	 */
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override; //限制属性的最大值和最小值，与上一个函数的区别是调用时机不同

	
private:
	UPROPERTY(ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	UPROPERTY(ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	UPROPERTY(ReplicatedUsing=OnRep_Mana)
	FGameplayAttributeData Mana;
	UPROPERTY(ReplicatedUsing=OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	UPROPERTY(ReplicatedUsing=OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;
	UPROPERTY(ReplicatedUsing=OnRep_Armor)
	FGameplayAttributeData Armor;
	UPROPERTY(ReplicatedUsing=OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	
	//函数被调用时，服务端向客户端拷贝新的数据，且仍会提供旧的数据供你使用，这样新值旧值都可以使用
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_AttackDamage(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);
};
