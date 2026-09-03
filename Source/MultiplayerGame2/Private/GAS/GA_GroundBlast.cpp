// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_GroundBlast.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CAbilitySystemStatics.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor_GroundPick.h"

UGA_GroundBlast::UGA_GroundBlast()
{
	ActivationOwnedTags.AddTag(UCAbilitySystemStatics::GetAimStatTag()); //技能激活时，添加tag
	BlockAbilitiesWithTag.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag()); //添加中断ability的tag
}

void UGA_GroundBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
                                      const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) //权限检查
	{
		return;
	}
	
	//播放蒙太奇
	UAbilityTask_PlayMontageAndWait* PlayGroundBlastAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,
		NAME_None, TargetMontage);
	PlayGroundBlastAnimTask->OnBlendOut.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlastAnimTask->OnCancelled.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlastAnimTask->OnInterrupted.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlastAnimTask->OnCompleted.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlastAnimTask->ReadyForActivation();
	
	//目标选择task
	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, 
		EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	WaitTargetDataTask->ValidData.AddDynamic(this, &UGA_GroundBlast::TargetConfirmed); //ValidData接收广播事件
	WaitTargetDataTask->Cancelled.AddDynamic(this, &UGA_GroundBlast::TargetCanceled);
	WaitTargetDataTask->ReadyForActivation();
	AGameplayAbilityTargetActor* TargetActor; //目标actor
	WaitTargetDataTask->BeginSpawningActor(this, TargetActorClass, TargetActor); //生成目标actor
	ATargetActor_GroundPick* GroundPickActor = Cast<ATargetActor_GroundPick>(TargetActor); //用于调试的类型转换
	if (GroundPickActor)
	{
		GroundPickActor->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPickActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickActor->SetTargetTraceRange(TargetTraceRange);
	}
	WaitTargetDataTask->FinishSpawningActor(this, TargetActor); //完成生成目标actor
}

void UGA_GroundBlast::TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(TargetDataHandle, DamageEffectDef.DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)); //伤害目标
		PushTargets(TargetDataHandle, DamageEffectDef.PushVelocity); //击飞所有目标
	}
	//代码调用GameplayCue
	FGameplayCueParameters BlastingCueParams; //定义GC
	BlastingCueParams.Location = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint; //从TargetDataHandle获得特效生成位置
	BlastingCueParams.RawMagnitude = TargetAreaRadius; //爆炸特效范围，基于技能半径
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(BlastCueTag, BlastingCueParams); //执行爆炸特效GC
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), BlastingCueParams); //执行相机震动GC，自动向client同步
	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
		OwnerAnimInstance->Montage_Play(CastMontage);
	K2_EndAbility();
}

void UGA_GroundBlast::TargetCanceled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// UE_LOG(LogTemp, Warning, TEXT("Target canceled"));
	K2_EndAbility();
}
