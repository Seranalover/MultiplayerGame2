// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
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
	
	UPROPERTY(meta=(BindWidget))
	class UWidget* MainWidgetRoot;
	
	void SwitchToMainMenuWidget(); //切换到主菜单
	
	/******************************************/
	/*                Session                 */
	/******************************************/
private:
	UPROPERTY(meta=(BindWidget))
	class UButton* CreateSessionBtn;
	
	UPROPERTY(meta=(BindWidget))
	class UEditableText* NewSessionNameText;
	
	UFUNCTION()
	void CreateSessionBtnClicked();
	
	UFUNCTION()
	void NewSessionNameTextChanged(const FText& NewText);
	
	UFUNCTION()
	void CancelSessionCreation();

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
	
	/******************************************/
	/*                 Waiting                */
	/******************************************/
private:
	UPROPERTY(meta=(BindWidget))
	class UWaitingWidget* WaitingWidget;
	
	FOnButtonClickedEvent& SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowCancel = false);
};
