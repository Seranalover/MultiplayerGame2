// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_UpperCut.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

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
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = 
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UpperCutMontage);
		PlayMontageAndWait->OnBlendOut.AddDynamic(this, &UGA_UpperCut::K2_EndAbility);
		PlayMontageAndWait->OnCancelled.AddDynamic(this, &UGA_UpperCut::K2_EndAbility);
		PlayMontageAndWait->OnInterrupted.AddDynamic(this, &UGA_UpperCut::K2_EndAbility);
		PlayMontageAndWait->OnCompleted.AddDynamic(this, &UGA_UpperCut::K2_EndAbility);
		PlayMontageAndWait->ReadyForActivation();
	}
}
