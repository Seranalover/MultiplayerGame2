// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoTypes.generated.h"

class APlayerState;
class UPA_CharacterDefination;
/**
 * 结构体 - 玩家已选择队伍信息
 */
USTRUCT()
struct FPlayerSelection
{
	GENERATED_BODY()
	
public:
	FPlayerSelection();
	FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState);
	
	FORCEINLINE void SetSlot(uint8 NewSlot) { Slot = NewSlot; }
	FORCEINLINE uint8 GetPlayerSlot() const { return Slot; }
	FORCEINLINE FUniqueNetIdRepl GetPlayerUniqueId() const { return PlayerUniqueId; }
	FORCEINLINE FString GetPlayerNickName() const { return PlayerNickName; }
	FORCEINLINE const UPA_CharacterDefination* GetCharacterDefinition() const { return CharacterDefinition; }
	FORCEINLINE void SetCharacterDefinition(const UPA_CharacterDefination* NewCharacterDefinition) { CharacterDefinition = NewCharacterDefinition; }
	
	bool IsForPlayer(const APlayerState* PlayerState) const;
	bool IsValid() const;
	static uint8 GetInvalidSlot();
	
private:
	UPROPERTY()
	uint8 Slot; //玩家在队伍中的槽位号
	
	UPROPERTY()
	FUniqueNetIdRepl PlayerUniqueId; //玩家在游戏内部网络中的唯一标识
	
	UPROPERTY()
	FString PlayerNickName;
	
	UPROPERTY()
	const UPA_CharacterDefination* CharacterDefinition;
};
