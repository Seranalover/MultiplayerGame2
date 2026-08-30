// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerInfoTypes.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Network/CNetStatics.h"

FPlayerSelection::FPlayerSelection()
	:Slot{GetInvalidSlot()}, PlayerUniqueId{FUniqueNetIdRepl::Invalid()}, PlayerNickName{}, CharacterDefinition{nullptr}
{
}

FPlayerSelection::FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState)
	:Slot{InSlot}, CharacterDefinition{nullptr}
{
	if(InPlayerState)
	{
		PlayerUniqueId = InPlayerState->GetUniqueId();
		PlayerNickName = InPlayerState->GetPlayerName();
	}
}

bool FPlayerSelection::IsForPlayer(const APlayerState* PlayerState) const
{
	if (!PlayerState) return false;

	#if WITH_EDITOR
		return PlayerState->GetPlayerName() == PlayerNickName; //如果当前处于编辑模式（开发者）
	#else
		return PlayerState->GetUniqueId() == GetPlayerUniqueId(); //正常玩家
	#endif
}

bool FPlayerSelection::IsValid() const
{
	#if WITH_EDITOR //编辑模式下执行
		return true;
	#else //非编辑模式下执行，例如服务器模式
		if (!PlayerUniqueId.IsValid()) 
			return false;
	
		if (Slot == GetInvalidSlot()) 
			return false;
	
		if (Slot >= UCNetStatics::GetPlayerCountPerTeam() * 2)	
			return false;
	
		return true;
	#endif
}

uint8 FPlayerSelection::GetInvalidSlot()
{
	return uint8(255);
}
