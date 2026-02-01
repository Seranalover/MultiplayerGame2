// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AN_SendGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"

void UAN_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp->GetOwner()) return;
	
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!OwnerASC) return;
	
	//向actor发送标识为tag的事件
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
}

//重写获得通知名方法，将通知名替换为Tag名，方便查看和管理
FString UAN_SendGameplayEvent::GetNotifyName_Implementation() const
{
	if (EventTag.IsValid())
	{
		TArray<FName> TagNames;
		UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames); //将tag拆分成数组
		return TagNames.Last().ToString(); //返回tag的最后一个节点文本
	}
	return "None";
}
