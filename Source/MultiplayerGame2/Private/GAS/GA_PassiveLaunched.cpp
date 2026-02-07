// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_PassiveLaunched.h"

#include "CAbilitySystemStatics.h"

UGA_PassiveLaunched::UGA_PassiveLaunched()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly; //该技能仅在Server触发，避免客户端操作不当产生bug，更安全
	
	ActivationBlockedTags.RemoveTag(UCAbilitySystemStatics::GetStunStatTag()); //移除stun tag
	
	FAbilityTriggerData TriggerData; //定义一个Trigger Data
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent; //设置trigger source
	TriggerData.TriggerTag = GetLaunchedAbilityTriggerTag(); //设置trigger tag
	AbilityTriggers.Add(TriggerData); //设置当前技能触发条件
}

void UGA_PassiveLaunched::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	if (K2_HasAuthority()) //server only
	{
		PushSelf(TriggerEventData->TargetData.Get(0)->GetHitResult()->ImpactNormal); //从TriggerEventData中获取击中的方向
		K2_EndAbility();
	}
}

FGameplayTag UGA_PassiveLaunched::GetLaunchedAbilityTriggerTag()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.launch.activate");
}
