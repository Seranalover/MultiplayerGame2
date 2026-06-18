// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerInfoTypes.h"
#include "CGameState.generated.h"

class UPA_CharacterDefination;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSelectionUpdated, const TArray<FPlayerSelection>& /*NewPlayerSelection*/)
/**
 * 游戏状态类
 * 作用是在大厅保存玩家的队伍选择状态
 */
UCLASS()
class ACGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	FOnPlayerSelectionUpdated OnPlayerSelectionUpdated;
	
	void RequestPlayerSelectionChange(const APlayerState* RequestingPlayer, uint8 DesiredSlot);
	bool IsSlotOccupied(uint8 SlotId) const; //槽位已被占用？
	const TArray<FPlayerSelection>& GetPlayerSelectionArray() const;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	bool CanStartHeroSelection() const;
	void SetCharacterSelected(const APlayerState* SelectingPlayer, const UPA_CharacterDefination* SelectedDefinition);
	bool IsDefinitionSelected(const UPA_CharacterDefination* SelectedDefinition) const;
	void SetCharacterDeselected(const UPA_CharacterDefination* DeselectedDefinition);
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerSelectionArray)
	TArray<FPlayerSelection> PlayerSelectionArray;
	
	UFUNCTION()
	void OnRep_PlayerSelectionArray();
};
