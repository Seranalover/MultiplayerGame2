// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CCharacter.h"
#include "Minion.generated.h"

/**
 * 小兵类
 */
UCLASS()
class AMinion : public ACCharacter
{
	GENERATED_BODY()
	
public:
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	bool IsActive() const; //是否存活？
	void Activate(); //存活
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	TMap<FGenericTeamId, USkeletalMesh*> SkinMap; //皮肤和队伍映射集
	
	void PickSkinBasedOnTeamID(); //根据队伍设置不同皮肤（骨骼）
	virtual void OnRep_TeamID() override;
};
