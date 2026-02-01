// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AM_SendTargetGroup.h"

#include "AbilitySystemBlueprintLibrary.h"

void UAM_SendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp) return;
	if (TargetSocketNames.Num() <= 1) return; //至少需要两个插槽才能确定一条扫描轨迹
	if (!MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner())) return;
	
	FGameplayEventData EventData;
	for (int i = 1; i < TargetSocketNames.Num(); ++i)
	{
		FGameplayAbilityTargetData_LocationInfo* LocationInfo = new FGameplayAbilityTargetData_LocationInfo();
		FVector SourceLocation = MeshComp->GetSocketLocation(TargetSocketNames[i-1]);
		FVector TargetLocation = MeshComp->GetSocketLocation(TargetSocketNames[i]);
		LocationInfo->SourceLocation.LiteralTransform.SetLocation(SourceLocation); //扫描开始位置
		LocationInfo->TargetLocation.LiteralTransform.SetLocation(TargetLocation); //扫描结束位置
		
		EventData.TargetData.Add(LocationInfo); //保存扫描位置信息到事件
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, EventData);
}
