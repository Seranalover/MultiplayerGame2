// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Combo.h"

void UGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	/**
	 * 重要函数CommitAbility
	 * 它主要做两件事，一是是否满足技能释放条件，例如是否在冷却或者法力值是否足够，二是它会提交资源消耗，并重置冷却
	 * 如果一切顺利，它会返回执行结构true，否则返回false
	 */
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Multiplayergame2 Error: Casting GA Combo"))
}
