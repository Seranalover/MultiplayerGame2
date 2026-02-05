// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
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
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	class UAIPerceptionComponent* AiPerceptionComponent; //ai感知组件
	
	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	class UAISenseConfig_Sight* SightConfig; //视线配置
	
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	class UBehaviorTree* BehaviorTree; //ai行为树
	
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	FName TargetBlackboardKeyName = "Target";
	
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus);
	
	const UObject* GetCurrentTarget() const; //当前追逐目标
	void SetCurrentTarget(AActor* TargetActor);
};
