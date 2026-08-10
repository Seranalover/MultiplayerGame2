// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LobbyGameMode.h"

#include "Network/CGameSession.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true; //非常重要！开启后才能实现玩家状态复制，将当前玩家的状态数据传递至下一个玩家状态
	GameSessionClass = ACGameSession::StaticClass();
}
