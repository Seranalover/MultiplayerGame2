// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameInstance.h"

#include "Network/CNetStatics.h"

void UCGameInstance::StartMatch()
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer || GetWorld()->GetNetMode() == ENetMode::NM_ListenServer)
		LoadLevelAndListen(GameLevel);
}

void UCGameInstance::Init()
{
	Super::Init();
	if (GetWorld()->IsEditorWorld()) return;
	if (UCNetStatics::IsSessionServer(this))
	{
		CreateSession();	
	}
}

void UCGameInstance::CreateSession()
{
	ServerSessionName = UCNetStatics::GetSessionNameStr();
	FString SessionSearchId = UCNetStatics::GetSessionSearchIdStr();
	ServerSessionPort = UCNetStatics::GetSessionPort();
	
	UE_LOG(LogTemp, Warning, TEXT("### Create Session with Name: %s, ID: %s, Port: %d"), *ServerSessionName, *SessionSearchId, ServerSessionPort);
}

void UCGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	
	if (LevelURL != "")
		GetWorld()->ServerTravel(LevelURL.ToString() + "?listen");
}
