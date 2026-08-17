// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "CNetStatics.generated.h"

/**
 * 静态工具函数库类
 */
UCLASS()
class UCNetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static uint8 GetPlayerCountPerTeam(); //获得每个队伍的玩家数量
	
	static bool IsSessionServer(const UObject* WorldContextObject); //当前是否为服务器
	
	static FString GetSessionNameStr(); //获得session name
	static FName GetSessionNameKey();
	
	static FString GetSessionSearchIdStr(); //获得session search id
	static FName GetSessionSearchIdKey();
	
	static int GetSessionPort(); //获得端口号
	static FName GetPortKey();
	
	static IOnlineSessionPtr GetSessionPtr(); //获得EOS Session ptr
	static IOnlineIdentityPtr GetIdentityPtr(); //获得EOS id ptr
	static FOnlineSessionSettings GenerateOnlineSessionSettings(const FName& SessionName, const FString& SessionSearchId, int Port); //设置EOS配置
	
	static FString GetCommandLineArgsAsString(const FName& ParamName); //解析命令参数为字符类型
	static int GetCommandLineArgsAsInt(const FName& ParamName); //解析命令参数为数字类型

	/**
	 * Coordinator: 协调器，本质是一个小型服务器或者说是监听Http请求的小型脚本，负责协调新服务器实例的创建
	 */
	static FName GetCoordinatorURLKey(); //获得协调器url key
	static FString GetCoordinatorURL(); //获得协调器url
	static FString GetDefaultCoordinatorURL(); //协调器url为空时，从配置文件获得默认协调器url
	
	static FString GetTestingURL(); //获得测试url
	static FName GetTestingURLKey();
	
	static void ReplacePort(FString& OutURL, int NewPort); //重设url端口
};
