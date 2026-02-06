// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Minion.h"

#include "AbilitySystemComponent.h"
#include "GAS/CAbilitySystemStatics.h"

//重写SetGenericTeamId（）确保在监听服务器也能正常使用
void AMinion::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	Super::SetGenericTeamId(TeamID);
	PickSkinBasedOnTeamID();
}

bool AMinion::IsActive() const
{
	return !GetAbilitySystemComponent()->HasMatchingGameplayTag(UCAbilitySystemStatics::GetDeadStatTag()); //是否存活？
}

void AMinion::Activate()
{
	GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(UCAbilitySystemStatics::GetDeadStatTag())); //移除dead tag
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
