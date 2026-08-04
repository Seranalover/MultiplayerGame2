// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameModeBase.h"
#include "EngineUtils.h"
#include "StormCore.h"
#include "GameFramework/PlayerStart.h"
#include "Player/CPlayerController.h"
#include "Player/CPlayerState.h"

APlayerController* ACGameModeBase::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
	IGenericTeamAgentInterface* NewPlayerTeamAgentInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);
	FGenericTeamId NewTeamId = GetTeamIdForPlayer(NewPlayerController); //生成team id
	if (NewPlayerTeamAgentInterface)
	{
		NewPlayerTeamAgentInterface->SetGenericTeamId(NewTeamId); //设置team id
	}
	NewPlayerController->StartSpot = FindNextStartSpotForTeam(NewTeamId); //分配出生点
	return NewPlayerController;
}

void ACGameModeBase::StartPlay()
{
	Super::StartPlay();
	AStormCore* StormCore = GetStormCore();
	if (StormCore)
	{
		StormCore->OnGoalReachedDelegate.AddUObject(this, &ACGameModeBase::MatchFinished);
	}
}

UClass* ACGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	ACPlayerState* CPlayerState = InController->GetPlayerState<ACPlayerState>();
	if (CPlayerState && CPlayerState->GetSelectedPawnClass())
		return CPlayerState->GetSelectedPawnClass();
	return BackupPawn;
}

APawn* ACGameModeBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayer);
	FGenericTeamId TeamId = GetTeamIdForPlayer(NewPlayer);
	if (NewPlayerTeamInterface)
		NewPlayerTeamInterface->SetGenericTeamId(TeamId);
	StartSpot = FindNextStartSpotForTeam(TeamId);
	NewPlayer->StartSpot = StartSpot;
	
	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}

FGenericTeamId ACGameModeBase::GetTeamIdForPlayer(const AController* InController) const
{
	ACPlayerState* CPlayerState = InController->GetPlayerState<ACPlayerState>();
	if (CPlayerState && CPlayerState->GetSelectedPawnClass())
		return CPlayerState->GetTeamIdBasedOnSlot();
	
	static int PlayerCount = 0;
	++PlayerCount;
	return FGenericTeamId(PlayerCount % 2);
}

AActor* ACGameModeBase::FindNextStartSpotForTeam(const FGenericTeamId& TeamId) const
{
	const FName* StartSpotTag = TeamStartSpotTagMap.Find(TeamId);
	if (!StartSpotTag) return nullptr;
	
	UWorld* World = GetWorld(); //获取世界迭代器
	for (TActorIterator<APlayerStart> It(World); It; ++It) //遍历玩家出生点
	{
		if (It->PlayerStartTag == *StartSpotTag)
		{
			It->PlayerStartTag = FName("Taken");
			return *It;
		}
	}
	
	return nullptr;
}

class AStormCore* ACGameModeBase::GetStormCore() const
{
	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<AStormCore> It(World); It; ++It)
		{
			return *It;
		}
	}
	return nullptr;
}

void ACGameModeBase::MatchFinished(AActor* ViewTarget, int WinningTeam)
{
	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<ACPlayerController> It(World); It; ++It)
		{
			It->MatchFinished(ViewTarget, WinningTeam);
		}
	}
}