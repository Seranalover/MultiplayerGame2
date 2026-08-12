// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 主菜单界面控件类
 */
UCLASS()
class UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
/******************************************/
/*                  Main                  */
/******************************************/
private:
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* MainSwitcher;
	
	UPROPERTY()
	class UCGameInstance* CGameInstance;

/******************************************/
/*                 Login                  */
/******************************************/
private:
	UPROPERTY(meta=(BindWidget))
	class UWidget* LoginWidgetRoot;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* LoginButton;
	
	UFUNCTION()
	void LoginBtnClicked();
	
	void LoginCompleted(bool bWasSuccessful, const FString& PlayerNickName, const FString& ErrorMsg);
};
