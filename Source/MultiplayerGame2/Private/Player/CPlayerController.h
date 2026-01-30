// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

/**
 * 玩家控制器基类
 */
UCLASS()
class ACPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void OnPossess(APawn* NewPawn) override; //仅在服务端会被调用
	void AcknowledgePossession(class APawn* NewPawn) override; //仅在客户端会被调用，P2P的监听服务器也会被调用
	
private:
	UPROPERTY()
	class ACPlayerCharacter* CPlayerCharacter;
};
