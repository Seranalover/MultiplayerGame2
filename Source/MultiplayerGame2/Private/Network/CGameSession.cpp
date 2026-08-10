// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/CGameSession.h"

#include "Framework/CGameInstance.h"


bool ACGameSession::ProcessAutoLogin()
{
	return true;
}

void ACGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
	if (UCGameInstance* GameInstance = GetGameInstance<UCGameInstance>())
	{
		GameInstance->PlayerJoined(UniqueId);
	}
}

void ACGameSession::UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId)
{
	Super::UnregisterPlayer(InSessionName, UniqueId);
	if (UCGameInstance* GameInstance = GetGameInstance<UCGameInstance>())
	{
		GameInstance->PlayerLeft(UniqueId);
	}
}
