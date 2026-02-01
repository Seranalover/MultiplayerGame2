// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SendGameplayEvent.generated.h"

/**
 * 发送游戏事件的动画通知类
 */
UCLASS()
class UAN_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

private:
	UPROPERTY(EditAnywhere, Category="Gameplay Ability")
	FGameplayTag EventTag; //事件Tag
	
	virtual FString GetNotifyName_Implementation() const override; //重写获得通知名方法，将通知名替换为Tag名，方便查看和管理

};
