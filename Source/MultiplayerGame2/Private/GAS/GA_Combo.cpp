// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Combo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	/**
	 * 重要函数CommitAbility
	 * 它主要做两件事，一是是否满足技能释放条件，例如是否在冷却或者法力值是否足够，二是它会提交资源消耗，并重置冷却
	 * 如果一切顺利，它会返回执行结构true，否则返回false
	 */
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	
	// UE_LOG(LogTemp, Warning, TEXT("Multiplayergame2 Error: Casting GA Combo"))
	
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = 
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage); //创建蒙太奇task
		//动画淡出、取消、结束、被打断时，结束combo技能
		PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		//task等待执行，必须调用，否则task不会执行！
		PlayMontageAndWaitTask->ReadyForActivation();
	}
}
