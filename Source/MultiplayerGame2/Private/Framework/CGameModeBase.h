// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/GameModeBase.h"
#include "CGameModeBase.generated.h"

/**
 * 游戏模式基类
 */
UCLASS()
class ACGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;

private:
	FGenericTeamId GetTeamIdForPlayer(const APlayerController* PlayerController) const; //生成team id
	
	AActor* FindNextStartSpotForTeam(const FGenericTeamId& TeamId) const;
	
	UPROPERTY(EditDefaultsOnly, Category="Team")
	TMap<FGenericTeamId, FName> TeamStartSpotTagMap; //玩家出生点集合
};
