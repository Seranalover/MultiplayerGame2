// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CGameInstance.generated.h"

/**
 * 游戏实例类
 * 在游戏运行期间持续存在，伴随整个游戏进程
 * 可用于在其基础上构建网络功能，涉及到大厅的创建和epic在线服务的连接
 * 可用于实现地图加载功能
 */
UCLASS()
class UCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void StartMatch(); //点击 开始游戏 时加载游戏关卡
	virtual void Init() override; //游戏启动时调用

/*****************************************************/
/*                  Session Server                   */
/*****************************************************/
private:
	FString ServerSessionName;
	int ServerSessionPort;
	void CreateSession();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuLevel; //使用软引用，任何硬引用都会将资源整个加载到内存中
	
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> LobbyLevel;
	
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevel;
	
	void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);
};
