// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "CGameplayAbility.generated.h"

/**
 * 能力基础类
 */
UCLASS()
class UCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	class UAnimInstance* GetOwnerAnimInstance() const; //动画实例
	
	TArray<FHitResult> GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, 
		float SphereSweepRadius = 30.0f, ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, bool bDrawDebug = false, 
		bool bIgnoreSelf = true) const; //Target插槽扫描结果
	
	FORCEINLINE bool ShouldDrawDebug() const { return bShouldDrawDebug; }
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bShouldDrawDebug = false;
};
