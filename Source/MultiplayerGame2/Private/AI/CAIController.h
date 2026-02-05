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
	void TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus); //感知目标更新时
	UFUNCTION()
	void TargetForgotten(AActor* ForgottenActor); //感知目标被遗忘
	
	const UObject* GetCurrentTarget() const; //当前追逐目标
	void SetCurrentTarget(AActor* TargetActor); //设置当前追逐目标
	AActor* GetNextPerceivedActor() const; //感知到的下一个目标
	void ForgetActorIfDead(AActor* Actor); //目标死亡不必等待时间，立即遗忘
};
