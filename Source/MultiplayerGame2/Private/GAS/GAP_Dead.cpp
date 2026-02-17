// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GAP_Dead.h"

#include "CAbilitySystemStatics.h"

UGAP_Dead::UGAP_Dead()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly; //设置为server only
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = UCAbilitySystemStatics::GetDeadStatTag();
	AbilityTriggers.Add(TriggerData);
	ActivationBlockedTags.RemoveTag(UCAbilitySystemStatics::GetStunStatTag());
}

void UGAP_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (K2_HasAuthority())
	{
		AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser(); //获得击杀者
		if (Killer)
			UE_LOG(LogTemp, Warning, TEXT("I am dead, the killer is: %s"), *Killer->GetName());
	}
}
