// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "CGameSession.generated.h"

/**
 * 游戏会话类
 * 用于管理用户会话连接
 */
UCLASS()
class ACGameSession : public AGameSession
{
	GENERATED_BODY()
	
public:
	virtual bool ProcessAutoLogin() override; //server only
	virtual void RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite) override; //当有玩家加入会话
	virtual void UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId) override; //当有玩家离开会话
};
