// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Minion.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

//重写SetGenericTeamId（）确保在监听服务器也能正常使用
void AMinion::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	Super::SetGenericTeamId(TeamID);
	PickSkinBasedOnTeamID();
}

bool AMinion::IsActive() const
{
	return !IsDead(); //是否存活？
}

void AMinion::Activate()
{
	RespawnImmediately();
}

void AMinion::SetGoal(AActor* Goal)
{
	if (AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		if (UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent())
		{
			Blackboard->SetValueAsObject(GoalBlackboardKeyName, Goal);
		}
	}
}

void AMinion::PickSkinBasedOnTeamID()
{
	USkeletalMesh** SkinMesh = SkinMap.Find(GetGenericTeamId());
	if (SkinMesh)
	{
		GetMesh()->SetSkeletalMesh(*SkinMesh);
	}
}

void AMinion::OnRep_TeamID()
{
	PickSkinBasedOnTeamID();
}
