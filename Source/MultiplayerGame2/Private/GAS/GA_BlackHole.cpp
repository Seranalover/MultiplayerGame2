// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_BlackHole.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor_GroundPick.h"

void UGA_BlackHole::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}
	
	//技能动画
	PlayCastBlackHoleMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	PlayCastBlackHoleMontageTask->OnBlendOut.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnCancelled.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnInterrupted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnCompleted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
	PlayCastBlackHoleMontageTask->ReadyForActivation();
	
	//瞄准
	UAbilityTask_WaitTargetData* WaitPlacementTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	WaitPlacementTask->ValidData.AddDynamic(this, &UGA_BlackHole::PlaceBlackHole);
	WaitPlacementTask->Cancelled.AddDynamic(this, &UGA_BlackHole::PlacementCancelled);
	WaitPlacementTask->ReadyForActivation();
	
	//生成瞄准器
	AGameplayAbilityTargetActor* TargetActor;
	WaitPlacementTask->BeginSpawningActor(this, TargetActorClass, TargetActor);
	ATargetActor_GroundPick* TargetActor_GroundPick = Cast<ATargetActor_GroundPick>(TargetActor);
	if (TargetActor_GroundPick)
	{
		TargetActor_GroundPick->SetShouldDrawDebug(ShouldDrawDebug());
		TargetActor_GroundPick->SetTargetAreaRadius(TargetAreaRadius);
		TargetActor_GroundPick->SetTargetTraceRange(TargetTraceRange);
	}
	WaitPlacementTask->FinishSpawningActor(this, TargetActor);
	AddAimEffect();
}

void UGA_BlackHole::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveAimEffect();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BlackHole::PlaceBlackHole(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
}

void UGA_BlackHole::PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
}

void UGA_BlackHole::AddAimEffect()
{
	AimEffectHandle = BP_ApplyGameplayEffectToOwner(AimEffect);
}

void UGA_BlackHole::RemoveAimEffect()
{
	if (AimEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(AimEffectHandle);
	}
}
