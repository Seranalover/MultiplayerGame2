// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GAP_Dead.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CAbilitySystemStatics.h"
#include "CHeroAttributeSet.h"
#include "Engine/OverlapResult.h"

UGAP_Dead::UGAP_Dead()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly; //设置为server only
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = UCAbilitySystemStatics::GetDeadStatTag();
	AbilityTriggers.Add(TriggerData);
	ActivationBlockedTags.RemoveTag(UCAbilitySystemStatics::GetStunStatTag());
}

void UGAP_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (K2_HasAuthority())
	{
		AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser(); //获得击杀者
		if (!Killer || !UCAbilitySystemStatics::IsHero(Killer))
			Killer = nullptr;
		TArray<AActor*> RewardActors = GetRewardActors();
		if (RewardActors.IsEmpty() || !Killer) //killer不存在，返回
		{
			K2_EndAbility();
			return;
		}
		if (Killer && !RewardActors.Contains(Killer)) //killer存在但不在奖励队列中
			RewardActors.Add(Killer); //添加killer到奖励队列，避免如全图技能击杀，killer不在范围内导致没有添加到奖励队列
		
		bool bFound = false;
		float SelfExperience = GetAbilitySystemComponentFromActorInfo_Ensured()->GetGameplayAttributeValue(UCHeroAttributeSet::GetExperienceAttribute(), bFound);
		float TotalExperienceReward = BaseExperienceReward + ExperienceRewardPerExperience * SelfExperience;
		float TotalGoldReward = BaseGoldReward + GoldRewardPerExperience * SelfExperience;
		
		if (Killer) //计算killer奖励
		{
			float KillerExperienceReward = TotalExperienceReward * KillerRewardPortion;
			float KillerGoldReward = TotalGoldReward * KillerRewardPortion;
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffect);
			EffectSpecHandle.Data->SetSetByCallerMagnitude(UCAbilitySystemStatics::GetExperienceAttributeTag(), KillerExperienceReward);
			EffectSpecHandle.Data->SetSetByCallerMagnitude(UCAbilitySystemStatics::GetGoldAttributeTag(), KillerGoldReward);
			K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer)); //应用GE
			
			TotalExperienceReward -= KillerExperienceReward;
			TotalGoldReward -= KillerGoldReward;
		}
		
		//计算助攻者奖励
		float ExperiencePerTarget = TotalExperienceReward / RewardActors.Num();
		float GoldPerTarget = TotalGoldReward / RewardActors.Num();
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffect);
		EffectSpecHandle.Data->SetSetByCallerMagnitude(UCAbilitySystemStatics::GetExperienceAttributeTag(), ExperiencePerTarget);
		EffectSpecHandle.Data->SetSetByCallerMagnitude(UCAbilitySystemStatics::GetGoldAttributeTag(), GoldPerTarget);
		K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(RewardActors, true));
		K2_EndAbility();
	}
}

TArray<AActor*> UGAP_Dead::GetRewardActors() const
{
	TSet<AActor*> RewardActors;
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !GetWorld()) return RewardActors.Array();
	
	FCollisionObjectQueryParams CollisionObjectParams; //碰撞参数
	CollisionObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionShape CollisionShape; //碰撞检测形状
	CollisionShape.SetSphere(RewardRange);
	TArray<FOverlapResult> OverlapResults; //碰撞结果
	if (GetWorld()->OverlapMultiByObjectType(OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity, 
		CollisionObjectParams, CollisionShape)) //如果检测到碰撞结果
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			const IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(OverlapResult.GetActor());
			if (!OtherTeamInterface || OtherTeamInterface->GetTeamAttitudeTowards(*AvatarActor) != ETeamAttitude::Hostile)
				continue; //跳过非友方单位
			if (!UCAbilitySystemStatics::IsHero(OverlapResult.GetActor()))
				continue; //跳过非英雄单位，如小兵
			RewardActors.Add(OverlapResult.GetActor());
		}
	}
	return RewardActors.Array();
}
