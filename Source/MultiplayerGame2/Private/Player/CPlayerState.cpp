// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerState.h"

#include "Framework/CGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACPlayerState::ACPlayerState()
{
	bReplicates = true;
	NetUpdateFrequency = 100.f; //该类更新频率很低，因此提升更新频率保证数据实时更新
}

void ACPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPlayerState, PlayerSelection);
}

void ACPlayerState::BeginPlay()
{
	Super::BeginPlay();
	CGameState = Cast<ACGameState>(UGameplayStatics::GetGameState(this));
	
	if (CGameState)
	{
		CGameState->OnPlayerSelectionUpdated.AddUObject(this, &ACPlayerState::PlayerSelectionUpdated);
	}
}

void ACPlayerState::Server_SetSelectedCharacterDefinition_Implementation(const UPA_CharacterDefination* NewDefinition)
{
	if (!CGameState) return;
	if (!NewDefinition) return;
	if (CGameState->IsDefinitionSelected(NewDefinition)) return;
	if (PlayerSelection.GetCharacterDefinition())
		CGameState->SetCharacterDeselected(PlayerSelection.GetCharacterDefinition());
	
	PlayerSelection.SetCharacterDefinition(NewDefinition);
	CGameState->SetCharacterSelected(this, NewDefinition);
}

bool ACPlayerState::Server_SetSelectedCharacterDefinition_Validate(const UPA_CharacterDefination* NewDefinition)
{
	return true;
}

void ACPlayerState::PlayerSelectionUpdated(const TArray<FPlayerSelection>& NewPlayerSelections)
{
	for (const FPlayerSelection& NewPlayerSelection : NewPlayerSelections)
	{
		if (NewPlayerSelection.IsForPlayer(this))
			PlayerSelection = NewPlayerSelection;
	}
}
