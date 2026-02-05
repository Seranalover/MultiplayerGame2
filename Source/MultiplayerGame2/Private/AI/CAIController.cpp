// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
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
	//ai感知目标更新时，绑定到TargetPerceptionUpdated()
	AiPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController::TargetPerceptionUpdated);
	AiPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ACAIController::TargetForgotten); //ai感知目标被遗忘时
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

void ACAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(BehaviorTree);
}

void ACAIController::TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed()) //感知到新目标？当前有目标，继续跟踪当前目标
	{
		if (!GetCurrentTarget()) 
			SetCurrentTarget(TargetActor); //当前没有目标，设置目标
	}
	else
	{
	}
		
}

void ACAIController::TargetForgotten(AActor* ForgottenActor)
{
	if (!ForgottenActor) return;
	
	if (GetCurrentTarget() == ForgottenActor)
		SetCurrentTarget(GetNextPerceivedActor()); //遗忘当前目标时，将感知到的下一个目标设置为追踪目标
}

const UObject* ACAIController::GetCurrentTarget() const
{
	const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent)
		return GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKeyName);
	return nullptr;
}

void ACAIController::SetCurrentTarget(AActor* TargetActor)
{
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (!BlackboardComponent) return;
	if (TargetActor)
		BlackboardComponent->SetValueAsObject(TargetBlackboardKeyName, TargetActor); //设置目标
	else
		BlackboardComponent->ClearValue(TargetBlackboardKeyName); //清除目标
}

AActor* ACAIController::GetNextPerceivedActor() const
{
	if (PerceptionComponent)
	{
		TArray<AActor*> Actors;
		AiPerceptionComponent->GetPerceivedHostileActors(Actors); //将感知到的敌人actor存入数组
		
		if (Actors.Num() != 0)
			return Actors[0];
	}
	return nullptr;
}
