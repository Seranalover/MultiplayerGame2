// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LobbyPlayerController.h"

#include "Framework/CGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Framework/CGameState.h"

void ALobbyPlayerController::Server_RequestSlotSelectionChange_Implementation(uint8 NewSlotId)
{
	if (!GetWorld()) return;
	
	ACGameState* CGameState = GetWorld()->GetGameState<ACGameState>();
	if (!CGameState) return;
	
	CGameState->RequestPlayerSelectionChange(GetPlayerState<APlayerState>(), NewSlotId);
}

bool ALobbyPlayerController::Server_RequestSlotSelectionChange_Validate(uint8 NewSlotId)
{
	return true;
}

void ALobbyPlayerController::Server_StartHeroSelection_Implementation()
{
	if (!HasAuthority() || !GetWorld()) return;
	
	for (FConstPlayerControllerIterator PlayerControllerIterator = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIterator; ++PlayerControllerIterator)
	{
		ALobbyPlayerController* PlayerController = Cast<ALobbyPlayerController>(*PlayerControllerIterator);
		if (PlayerController)
		{
			PlayerController->Client_StartHeroSelection();
		}
	}
}

bool ALobbyPlayerController::Server_StartHeroSelection_Validate()
{
	return true;
}

void ALobbyPlayerController::Client_StartHeroSelection_Implementation()
{
	OnSwitchToHeroSelection.ExecuteIfBound();
}

ALobbyPlayerController::ALobbyPlayerController()
{
	bAutoManageActiveCameraTarget = false; //无论切换到哪个控制实体，现在都不会更改为新的实体
}

void ALobbyPlayerController::Server_RequestStartMatch_Implementation()
{
	UCGameInstance* CGameInstance = GetGameInstance<UCGameInstance>();
	if (CGameInstance)
		CGameInstance->StartMatch();
}

bool ALobbyPlayerController::Server_RequestStartMatch_Validate()
{
	return true;
}
