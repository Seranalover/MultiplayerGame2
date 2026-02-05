// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CAIController.generated.h"

/**
 * 
 */
UCLASS()
class ACAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ACAIController();
	virtual void OnPossess(APawn* InPawn) override;
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	class UAIPerceptionComponent* AiPerceptionComponent; //ai感知组件
	
	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	class UAISenseConfig_Sight* SightConfig; //视线配置
};
