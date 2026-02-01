// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AM_SendTargetGroup.generated.h"

/**
 * 扫描点位置信息动画通知
 */
UCLASS()
class UAM_SendTargetGroup : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
		const FAnimNotifyEventReference& EventReference) override;
	
private:
	UPROPERTY(EditAnywhere, Category="Gameplay Ability")
	TArray<FName> TargetSocketNames; //插槽数组
	
	UPROPERTY(EditAnywhere, Category="Gameplay Ability")
	FGameplayTag EventTag; //事件tag

	
};
