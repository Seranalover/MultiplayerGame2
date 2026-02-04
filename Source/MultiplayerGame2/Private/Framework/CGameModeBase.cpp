// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameModeBase.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

APlayerController* ACGameModeBase::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
	IGenericTeamAgentInterface* NewPlayerTeamAgentInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);
	FGenericTeamId NewTeamId = GetTeamIdForPlayer(NewPlayerController);
	if (NewPlayerTeamAgentInterface)
	{
		NewPlayerTeamAgentInterface->SetGenericTeamId(NewTeamId);
	}
	NewPlayerController->StartSpot = FindNextStartSpotForTeam(NewTeamId); //分配出生点
	return NewPlayerController;
}

FGenericTeamId ACGameModeBase::GetTeamIdForPlayer(const APlayerController* PlayerController) const
{
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
