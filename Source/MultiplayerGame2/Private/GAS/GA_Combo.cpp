// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Combo.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CAbilitySystemStatics.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "GameplayTagsManager.h"

UGA_Combo::UGA_Combo()
{
	AbilityTags.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag()); //添加ability tag
	BlockAbilitiesWithTag.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag()); //添加block tag
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	/**
	 * 重要函数CommitAbility
	 * 它主要做两件事，一是是否满足技能释放条件，例如是否在冷却或者法力值是否足够，二是它会提交资源消耗，并重置冷却
	 * 如果一切顺利，它会返回执行结构true，否则返回false
	 */
	if (!K2_CommitAbility()) //CommitAbility已替换为封装好的K2_CommitAbility，作用相同
	{
		K2_EndAbility();
		return;
	}
	
	// UE_LOG(LogTemp, Warning, TEXT("Multiplayergame2 Error: Casting GA Combo"))
	
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)) //是否为服务端，或具有PredictionKey
	{
		//创建蒙太奇task，该task会自动向客户端同步，无需在客户端重复调用
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = 
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage); 
		//动画淡出、取消、结束、被打断时，结束combo技能
		PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		//task等待执行，必须调用，否则task不会执行！
		PlayMontageAndWaitTask->ReadyForActivation();
		
		//创建监听Gameplay Event的task，监听连招变化
		UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, GetComboChangedEventTag(), nullptr, false, false);
		//接收事件，绑定到GetComboChangedEventReceived()方法
		WaitGameplayEventTask->EventReceived.AddDynamic(this, &UGA_Combo::GetComboChangedEventReceived);
		WaitGameplayEventTask->ReadyForActivation(); //task等待执行
	}
	
	if (K2_HasAuthority()) //服务端？
	{
		UAbilityTask_WaitGameplayEvent* WaitTargetingEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, GetComboTargetEventTag()); //监听连击扫描事件动画通知
		WaitTargetingEventTask->EventReceived.AddDynamic(this, &UGA_Combo::DoDamage); //绑定到DoDamage()方法
		WaitTargetingEventTask->ReadyForActivation();
	}
	
	NextComboName = NAME_None;
	SetupWaitComboInputPress(); //尝试切换到下一段连招
}

FGameplayTag UGA_Combo::GetComboChangedEventTag()
{
	return FGameplayTag::RequestGameplayTag("ability.combo.change");
}

FGameplayTag UGA_Combo::GetComboChangedEventEndTag()
{
	return FGameplayTag::RequestGameplayTag("ability.combo.change.end");
}

FGameplayTag UGA_Combo::GetComboTargetEventTag()
{
	return FGameplayTag::RequestGameplayTag("ability.combo.damage");
}

void UGA_Combo::GetComboChangedEventReceived(FGameplayEventData Data)
{
	FGameplayTag EventTag = Data.EventTag;
	
	//重置连招
	if (EventTag == GetComboChangedEventEndTag()) 
	{
		NextComboName = NAME_None;
		// UE_LOG(LogTemp, Warning, TEXT("MultiplayerGame2 Error: next combo is cleared"));
		return;
	}
	
	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames); //将tag拆分成数组
	NextComboName = TagNames.Last(); //设置下一段连招名
	
	// UE_LOG(LogTemp, Warning, TEXT("MultiplayerGame2 Error: next combo is now: %s"), *NextComboName.ToString());
	// return TagNames.Last(); //返回tag的最后一个节点文本
		
}

void UGA_Combo::SetupWaitComboInputPress()
{
	UAbilityTask_WaitInputPress* WaitInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this); //创建等待按下输入task
	WaitInputPressTask->OnPress.AddDynamic(this, &UGA_Combo::HandleInputPress); //绑定到HandleInputPress()
	WaitInputPressTask->ReadyForActivation();
	
}

void UGA_Combo::HandleInputPress(float TimeWaited)
{
	SetupWaitComboInputPress(); //持续监听按下事件
	TryCommitCombo();
}

void UGA_Combo::TryCommitCombo()
{
	if (NextComboName == NAME_None) return;;
	
	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance(); //获得蒙太奇实例
	if (!OwnerAnimInstance) return;
	
	//设置连招下一段蒙太奇
	OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage), NextComboName, ComboMontage);
}

void UGA_Combo::DoDamage(FGameplayEventData Data)
{
	//扫描结果
	// TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(Data.TargetData, TargetSweepSphereRadius);
	
	// for (const FHitResult& HitResult : HitResults)
	// {
	// 	TSubclassOf<UGameplayEffect> GameplayEffect = GetDamageEffectForCurrentCombo(); //获得当前连段攻击效果
	// 	ApplyGameplayEffectToHitResult(HitResult, GameplayEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)); //对目标应用攻击效果
	// }
	int HitResultCount = UAbilitySystemBlueprintLibrary::GetDataCountFromTargetData(Data.TargetData);
	for (int i = 0; i < HitResultCount; i++)
	{
		FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Data.TargetData, i);
		TSubclassOf<UGameplayEffect> GameplayEffect = GetDamageEffectForCurrentCombo(); //获得当前连段攻击效果
		ApplyGameplayEffectToHitResult(HitResult, GameplayEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)); //对目标应用攻击效果
	}
}

TSubclassOf<UGameplayEffect> UGA_Combo::GetDamageEffectForCurrentCombo() const
{
	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance(); //获得动画啊实例
	if (OwnerAnimInstance)
	{
		FName CurrentComboSectionName = OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage); //获得当前连段名
		const TSubclassOf<UGameplayEffect>* FoundEffectPtr = DamageEffectMap.Find(CurrentComboSectionName); //按连段名查找效果集
		if (FoundEffectPtr) return *FoundEffectPtr;
	}
	
	return DefaultDamageEffect;
}
