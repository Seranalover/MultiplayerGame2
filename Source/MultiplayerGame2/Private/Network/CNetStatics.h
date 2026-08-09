// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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
	
	static bool IsSessionServer(const UObject* WorldContextObject);
	
	static FString GetSessionNameStr();
	static FName GetSessionNameKey();
	
	static FString GetSessionSearchIdStr();
	static FName GetSessionSearchIdKey();
	
	static int GetSessionPort();
	static FName GetPortKey();
	
	static FString GetCommandLineArgsAsString(const FName& ParamName); //解析命令参数为字符类型
	static int GetCommandLineArgsAsInt(const FName& ParamName); //解析命令参数为数字类型
};
