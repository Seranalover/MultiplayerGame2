// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Lazer.h"

#include "AbilitySystemComponent.h"
#include "CAttributeSet.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor_Line.h"

void UGA_Lazer::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !LazerMontage)
	{
		K2_EndAbility();
		return;
	}
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayLazerMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, LazerMontage);
		PlayLazerMontageTask->OnBlendOut.AddDynamic(this, &UGA_Lazer::K2_EndAbility);
		PlayLazerMontageTask->OnCancelled.AddDynamic(this, &UGA_Lazer::K2_EndAbility);
		PlayLazerMontageTask->OnInterrupted.AddDynamic(this, &UGA_Lazer::K2_EndAbility);
		PlayLazerMontageTask->OnCompleted.AddDynamic(this, &UGA_Lazer::K2_EndAbility);
		PlayLazerMontageTask->ReadyForActivation();
		
		UAbilityTask_WaitGameplayEvent* WaitShotEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetShootTag());
		WaitShotEvent->EventReceived.AddDynamic(this, &UGA_Lazer::ShootLazer);
		WaitShotEvent->ReadyForActivation();
		
		UAbilityTask_WaitCancel* WaitCancel = UAbilityTask_WaitCancel::WaitCancel(this);
		WaitCancel->OnCancel.AddDynamic(this, &UGA_Lazer::K2_EndAbility);
		WaitCancel->ReadyForActivation();
	}
}

void UGA_Lazer::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (OwnerASC && OnGoingConsumtionEffectHandle.IsValid())
	{
		OwnerASC->RemoveActiveGameplayEffect(OnGoingConsumtionEffectHandle);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FGameplayTag UGA_Lazer::GetShootTag()
{
	return FGameplayTag::RequestGameplayTag("ability.lazer.shoot");
}

void UGA_Lazer::ShootLazer(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		OnGoingConsumtionEffectHandle = BP_ApplyGameplayEffectToOwner(OnGoingConsumtionEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
		if (OwnerASC)
		{
			OwnerASC->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &UGA_Lazer::ManaUpdated);
		}
	}
	//生成目标选择器
	UAbilityTask_WaitTargetData* WaitTargetDataTask =
		UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, LazerTargetActorClass);
	WaitTargetDataTask->ValidData.AddDynamic(this, &UGA_Lazer::TargetReceived);
	WaitTargetDataTask->ReadyForActivation();
	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetDataTask->BeginSpawningActor(this, LazerTargetActorClass, TargetActor);
	ATargetActor_Line* LineTargetActor = Cast<ATargetActor_Line>(TargetActor);
	if (LineTargetActor)
		LineTargetActor->ConfigureTargetSetting(TargetRange, DetectionCylinderRadius, TargetingInterval, GetOwnerTeamId(), ShouldDrawDebug());
	WaitTargetDataTask->FinishSpawningActor(this, TargetActor);
	if (LineTargetActor)
		LineTargetActor->AttachToComponent(GetOwningComponentFromActorInfo(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TargetActorAttachSocketName);
	
}

void UGA_Lazer::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (OwnerASC && !OwnerASC->CanApplyAttributeModifiers(OnGoingConsumtionEffect.GetDefaultObject(), 
		GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo), MakeEffectContext(CurrentSpecHandle, CurrentActorInfo))) //CanApplyAttributeModifiers()判断是否允许拉取游戏效果，同时属性不能低于负值
	{
		K2_EndAbility();
	}
}

void UGA_Lazer::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(TargetDataHandle, HitDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	}
	PushTargets(TargetDataHandle, GetAvatarActorFromActorInfo()->GetActorForwardVector() * HitPushSpeed);
}
