// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Freeze.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CAbilitySystemStatics.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor_GroundPick.h"

UGA_Freeze::UGA_Freeze()
{
	ActivationOwnedTags.AddTag(UCAbilitySystemStatics::GetAimStatTag()); //技能激活时，添加tag
	BlockAbilitiesWithTag.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag()); //添加中断ability的tag
}

void UGA_Freeze::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		//播放瞄准动画
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetMontage);
		PlayMontageAndWait->OnBlendOut.AddDynamic(this, &UGA_Freeze::K2_EndAbility);
		PlayMontageAndWait->OnCancelled.AddDynamic(this, &UGA_Freeze::K2_EndAbility);
		PlayMontageAndWait->OnCompleted.AddDynamic(this, &UGA_Freeze::K2_EndAbility);
		PlayMontageAndWait->OnInterrupted.AddDynamic(this, &UGA_Freeze::K2_EndAbility);
		PlayMontageAndWait->ReadyForActivation();
		
		//选择目标
		UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, 
			EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
		WaitTargetData->ValidData.AddDynamic(this, &UGA_Freeze::TargetConfirmed);
		WaitTargetData->Cancelled.AddDynamic(this, &UGA_Freeze::TargetCanceled);
		WaitTargetData->ReadyForActivation();
		
		//生成选择器
		AGameplayAbilityTargetActor* TargetActor;
		WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);
		ATargetActor_GroundPick* GroundPickActor = Cast<ATargetActor_GroundPick>(TargetActor);
		if (GroundPickActor)
		{
			GroundPickActor->SetShouldDrawDebug(ShouldDrawDebug());
			GroundPickActor->SetTargetAreaRadius(TargetAreaRadius);
			GroundPickActor->SetTargetTraceRange(TargetTraceRange);
		}
		//结束生成
		WaitTargetData->FinishSpawningActor(this, TargetActor);
	}
}

void UGA_Freeze::TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(TargetDataHandle, DamageEffectDef.DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)); //伤害目标
	}
	
	//冰山特效
	FGameplayCueParameters FreezeCueParameters;
	FreezeCueParameters.Location = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint; //从TargetDataHandle获得特效生成位置
	FreezeCueParameters.RawMagnitude = TargetAreaRadius; //爆炸特效范围，基于技能半径
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(FreezeCueTag, FreezeCueParameters); //执行爆炸特效GC
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), FreezeCueParameters); //执行相机震动GC，自动向client同步
	
	//播放施法动画
	PlayMontageLocally(CastMontage);
	
	K2_EndAbility();
}

void UGA_Freeze::TargetCanceled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	K2_EndAbility();
}
