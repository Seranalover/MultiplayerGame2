// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "BTTask_SendInputToAbilitySystem.generated.h"

/**
 * 行为树发送Input ID 到 Ability System组件
 */
UCLASS()
class UBTTask_SendInputToAbilitySystem : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	//only do once
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	UPROPERTY(EditAnywhere, Category="Ability")
	ECAbilityInputID AbilityInputID;
};
