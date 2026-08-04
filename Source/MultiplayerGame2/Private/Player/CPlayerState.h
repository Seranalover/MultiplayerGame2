// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "PlayerInfoTypes.h"
#include "GameFramework/PlayerState.h"
#include "CPlayerState.generated.h"

class UPA_CharacterDefination;
/**
 * 玩家状态类
 * 作用是从游戏大厅传递状态数据到游戏场景中
 */
UCLASS()
class ACPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ACPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void CopyProperties(APlayerState* PlayerState) override; //在关卡中复制和传递玩家状态信息
	TSubclassOf<APawn> GetSelectedPawnClass() const;
	FGenericTeamId GetTeamIdBasedOnSlot() const; //根据slot位置获得team id
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSelectedCharacterDefinition(const UPA_CharacterDefination* NewDefinition);
	
private:
	UPROPERTY(Replicated)
	FPlayerSelection PlayerSelection;
	
	UPROPERTY()
	class ACGameState* CGameState;
	
	void PlayerSelectionUpdated(const TArray<FPlayerSelection>& NewPlayerSelections);
};
