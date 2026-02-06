// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Minion.h"

//重写SetGenericTeamId（）确保在监听服务器也能正常使用
void AMinion::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	Super::SetGenericTeamId(TeamID);
	PickSkinBasedOnTeamID();
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
