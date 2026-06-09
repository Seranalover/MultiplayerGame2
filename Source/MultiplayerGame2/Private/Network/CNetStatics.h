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
};
