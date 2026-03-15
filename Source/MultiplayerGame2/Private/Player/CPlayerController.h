// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

/**
 * 玩家控制器基类
 */
UCLASS()
class ACPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	void OnPossess(APawn* NewPawn) override; //仅在服务端会被调用
	void AcknowledgePossession(class APawn* NewPawn) override; //仅在客户端会被调用，P2P的监听服务器也会被调用
	
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override; //设置team id
	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override; //获得team id
	
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override; //管理数据生命周期并同步到客户端
	
	virtual void SetupInputComponent() override; //设置打开商店的输入映射
	
private:
	UPROPERTY()
	class ACPlayerCharacter* CPlayerCharacter;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class UGameplayWidget> GameplayWidgetClass; //控件基类
	
	UPROPERTY()
	class UGameplayWidget* GameplayWidget; //控件实例
	
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputMappingContext* UIInputMapping;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* ShopToggleInputAction;
	
	void SpawnGameplayWidget(); //生成GUI
	
	UFUNCTION()
	void ToggleShop();
};
