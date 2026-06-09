// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

/**
 * 主菜单玩家控制器
 */
UCLASS()
class AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override; //复制玩家状态
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Menu")
	TSubclassOf<UUserWidget> MenuWidgetClass;
	
	UPROPERTY()
	UUserWidget* MenuWidget;
	
	void SpawnWidget();
};
