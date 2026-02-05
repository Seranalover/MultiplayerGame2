// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ACAIController::ACAIController()
{
	AiPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component"); //创建感知组件
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config"); //创建视线组件
	
	SightConfig->DetectionByAffiliation.bDetectEnemies = true; //检测敌人？
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false; //检测友方？
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false; //检测中立单位？
	
	SightConfig->SightRadius = 1000.f; //视线半径
	SightConfig->LoseSightRadius = 1200.f; //视线丢失半径
	SightConfig->SetMaxAge(5.f); //丢失目标时继续跟踪的时间
	SightConfig->PeripheralVisionAngleDegrees = 180.f; //视线角度
	
	AiPerceptionComponent->ConfigureSense(*SightConfig); //配置视线参数
}

void ACAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	SetGenericTeamId(FGenericTeamId(0)); //初始化ai的team id
	
	IGenericTeamAgentInterface* PawnTeamAgentInterface = Cast<IGenericTeamAgentInterface>(InPawn);
	if (PawnTeamAgentInterface)
	{
		PawnTeamAgentInterface->SetGenericTeamId(GetGenericTeamId()); //设置ai的team id
	}
}
