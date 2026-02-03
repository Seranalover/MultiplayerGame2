// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"

#include "CAttributeSet.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	//监听attribute change，绑定HealthUpdated()函数
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &UCAbilitySystemComponent::HealthUpdated);
}

void UCAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return; //server only
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialEffects)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext()); 
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UCAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return; //server only
	
	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, (int32)AbilityPair.Key, nullptr)); //赋予附加能力
	}
	
	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& BasicAbilityPair : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(BasicAbilityPair.Value, 1, (int32)BasicAbilityPair.Key, nullptr)); //赋予基础能力
	}
}

void UCAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner()) return;
	
	if (ChangeData.NewValue <= 0 && GetOwner()->HasAuthority() && DeathEffect)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(DeathEffect, 1, MakeEffectContext()); 
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}
