// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AM_SendTargetGroup.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Kismet/KismetSystemLibrary.h"

void UAM_SendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp) return;
	if (TargetSocketNames.Num() <= 1) return; //至少需要两个插槽才能确定一条扫描轨迹
	if (!MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner())) return;
	
	FGameplayEventData EventData;
	TSet<AActor*> HitActors;
	AActor* OwnerActor = MeshComp->GetOwner();
	const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(OwnerActor);
	for (int i = 1; i < TargetSocketNames.Num(); ++i)
	{
		// FGameplayAbilityTargetData_LocationInfo* LocationInfo = new FGameplayAbilityTargetData_LocationInfo();
		FVector SourceLocation = MeshComp->GetSocketLocation(TargetSocketNames[i-1]);
		FVector TargetLocation = MeshComp->GetSocketLocation(TargetSocketNames[i]);
		// LocationInfo->SourceLocation.LiteralTransform.SetLocation(SourceLocation); //扫描开始位置
		// LocationInfo->TargetLocation.LiteralTransform.SetLocation(TargetLocation); //扫描结束位置
		// EventData.TargetData.Add(LocationInfo); //保存扫描位置信息到事件
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		TArray<FHitResult> HitResults;
		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreOwner) 
			ActorsToIgnore.Add(OwnerActor);
		EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
		UKismetSystemLibrary::SphereTraceMultiForObjects(MeshComp, SourceLocation, TargetLocation, SphereSweepRadius, 
			ObjectTypes, false, ActorsToIgnore, DrawDebugTrace, HitResults, false);
		for (const FHitResult& HitResult : HitResults)
		{
			if (HitActors.Contains(HitResult.GetActor()))
				continue;
			if (OwnerTeamInterface)
			{
				if (OwnerTeamInterface->GetTeamAttitudeTowards(*HitResult.GetActor()) != TargetTeam)
					continue;
			}
			FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
			EventData.TargetData.Add(TargetData);
			SendLocalGameplayCue(HitResult);
		}
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, EventData);
}

void UAM_SendTargetGroup::SendLocalGameplayCue(const FHitResult& HitResult) const
{
	FGameplayCueParameters CueParam;
	CueParam.Location = HitResult.ImpactPoint;
	CueParam.Normal = HitResult.ImpactNormal;
	
	for (const FGameplayTag& GameplayCueTag : TriggerGameplayCueTags)
	{
		UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(HitResult.GetActor(), GameplayCueTag, 
			EGameplayCueEvent::Executed, CueParam);
	}
}
