// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GAP_Dead.h"

#include "CAbilitySystemStatics.h"
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
		if (Killer)
			UE_LOG(LogTemp, Warning, TEXT("I am dead, the killer is: %s"), *Killer->GetName());
		TArray<AActor*> RewardActors = GetRewardActors();
		for (AActor* RewardActor : RewardActors)
		{
			UE_LOG(LogTemp, Warning, TEXT("find reward target: %s"), *RewardActor->GetName());
		}
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
