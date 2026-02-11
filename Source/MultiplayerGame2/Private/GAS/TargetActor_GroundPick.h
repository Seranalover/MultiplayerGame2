// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_GroundPick.generated.h"

/**
 * 地面技能目标选择器
 * 本质上属于Actor
 * 具备一定程度的网络同步能力，并非完全同步
 */
UCLASS()
class ATargetActor_GroundPick : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	ATargetActor_GroundPick();
	void SetTargetAreaRadius(float NewRadius);
	/** Requesting targeting data, but not necessarily stopping/destroying the task. Useful for external target data requests. */
	virtual void ConfirmTargetingAndContinue() override;
	void SetTargetOptions(bool bTargetFriendly, bool bTargetEnemy = true); //设置是否选取敌方或友方单位
	
protected:
	FVector GetTargetActorLocation() const; //获得目标选择器的位置，即玩家视线瞄准位置
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetRange = 2000.f; //技能射程半径
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 300.f; //选取半径
	
	bool bShouldTargetEnemy = true; //是否选取地方单位
	bool bShouldTargetFriendly = false; //是否选取友方单位
	
	virtual void Tick(float DeltaTime) override;
	
	
};
