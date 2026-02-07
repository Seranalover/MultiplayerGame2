// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_UpperCut.h"

#include "GameplayTagsManager.h"
#include "GA_Combo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void UGA_UpperCut::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		//类似combo逻辑处理
		//播放蒙太奇
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = 
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UpperCutMontage);
		PlayMontageAndWait->OnBlendOut.AddDynamic(this, &UGA_UpperCut::K2_EndAbility);
		PlayMontageAndWait->OnCancelled.AddDynamic(this, &UGA_UpperCut::K2_EndAbility);
		PlayMontageAndWait->OnInterrupted.AddDynamic(this, &UGA_UpperCut::K2_EndAbility);
		PlayMontageAndWait->OnCompleted.AddDynamic(this, &UGA_UpperCut::K2_EndAbility);
		PlayMontageAndWait->ReadyForActivation();
		
		//执行攻击事件
		UAbilityTask_WaitGameplayEvent* WaitGameplayEvent =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetUpperCutLaunchTag());
		WaitGameplayEvent->EventReceived.AddDynamic(this, &UGA_UpperCut::StartLaunching);
		WaitGameplayEvent->ReadyForActivation();
	}
	NextComboName = NAME_None;
}

FGameplayTag UGA_UpperCut::GetUpperCutLaunchTag()
{
	return FGameplayTag::RequestGameplayTag("ability.uppercut.launch");
}

void UGA_UpperCut::StartLaunching(FGameplayEventData EventData)
{
	if (K2_HasAuthority())
	{
		TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(EventData.TargetData, TargetSweepSphereRadius,
			ETeamAttitude::Hostile, ShouldDrawDebug()); //命中结果
		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * UpperLaunchVelocity); //自身浮空
		for (FHitResult& HitResult : HitResults)
		{
			PushTarget(HitResult.GetActor(), FVector::UpVector * UpperLaunchVelocity); //击飞敌人
			ApplyGameplayEffectToHitResult(HitResult, GameplayEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)); //对目标应用攻击效果
		}
	}
	
	//空中combo
	UAbilityTask_WaitGameplayEvent* WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,
		UGA_Combo::GetComboChangedEventTag(), nullptr, false, false);
	WaitGameplayEvent->EventReceived.AddDynamic(this, &UGA_UpperCut::HandleComboChangeEvent);
	WaitGameplayEvent->ReadyForActivation();
}

void UGA_UpperCut::HandleComboChangeEvent(FGameplayEventData EventData)
{
	FGameplayTag EventTag = EventData.EventTag;
	if (EventTag == UGA_Combo::GetComboChangedEventEndTag())
	{
		NextComboName = NAME_None;
		UE_LOG(LogTemp,Warning,TEXT("next combo is cleared"));
		return;
	}
	
	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	NextComboName = TagNames.Last();
	UE_LOG(LogTemp,Warning,TEXT("next combo is: %s"), *NextComboName.ToString());
}
