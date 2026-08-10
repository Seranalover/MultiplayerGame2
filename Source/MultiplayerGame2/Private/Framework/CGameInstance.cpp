// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameInstance.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
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
	IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr();
	if (SessionPtr)
	{
		ServerSessionName = UCNetStatics::GetSessionNameStr();
		FString SessionSearchId = UCNetStatics::GetSessionSearchIdStr();
		ServerSessionPort = UCNetStatics::GetSessionPort();
		UE_LOG(LogTemp, Warning, TEXT("### Create Session with Name: %s, ID: %s, Port: %d"), *ServerSessionName, *SessionSearchId, ServerSessionPort);
	
		FOnlineSessionSettings OnlineSessionSettings = UCNetStatics::GenerateOnlineSessionSettings(FName(ServerSessionName), SessionSearchId, ServerSessionPort);
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnSessionCreated);
		if (!SessionPtr->CreateSession(0, FName(ServerSessionName), OnlineSessionSettings))
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Creating Failed Right away!!!!."));
			SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
			TerminateSessionServer();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find session ptr, terminating..."));
		TerminateSessionServer();
	}
}

void UCGameInstance::OnSessionCreated(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("-------------------- Session Created!"));
		GetWorld()->GetTimerManager().SetTimer(WaitPlayerJoinTimeoutHandle, this, &UCGameInstance::WaitPlayerJoinTimeoutReached, WaitPlayerJoinTimeoutDuration);
		LoadLevelAndListen(LobbyLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("-------------------- Session Creation Failed!"));
		TerminateSessionServer();
	}
	
	//取消监听
	if (IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr())
	{
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::TerminateSessionServer()
{
	if (IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr())
	{
		SessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UCGameInstance::EndSessionCompleted);
		// SessionPtr->OnDestroySessionCompleteDelegates.RemoveAll(this);
		if (!SessionPtr->EndSession(FName{ServerSessionName}))
		{
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UCGameInstance::EndSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	FGenericPlatformMisc::RequestExit(false);
}

void UCGameInstance::WaitPlayerJoinTimeoutReached()
{
	UE_LOG(LogTemp, Warning, TEXT("Session Server shut down after %f seconds without player joining "), WaitPlayerJoinTimeoutDuration);
	TerminateSessionServer();
}

void UCGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	
	if (LevelURL != "")
	{
		FString TravelStr = FString::Printf(TEXT("%s?listen?port=%d"), *LevelURL.ToString(), ServerSessionPort);
		UE_LOG(LogTemp, Warning, TEXT("Server Traveling to: %s"), *TravelStr);
		GetWorld()->ServerTravel(TravelStr);
	}
}

void UCGameInstance::PlayerJoined(const FUniqueNetIdRepl& UniqueId)
{
	if (WaitPlayerJoinTimeoutHandle.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinTimeoutHandle);
	PlayerRecord.Add(UniqueId);
}

void UCGameInstance::PlayerLeft(const FUniqueNetIdRepl& UniqueId)
{
	PlayerRecord.Remove(UniqueId);
	if (PlayerRecord.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("All player left the session, terminating..."));
		TerminateSessionServer();
	}
}
