// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenuWidget.h"
#include "Framework/CGameInstance.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Widgets/WaitingWidget.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CGameInstance = GetGameInstance<UCGameInstance>();
	if (CGameInstance)
	{
		CGameInstance->OnLoginCompleted.AddUObject(this, &UMainMenuWidget::LoginCompleted);
		if (CGameInstance->IsLoggedIn())
		{
			SwitchToMainMenuWidget();
		}
	}
	
	LoginButton->OnClicked.AddDynamic(this, &UMainMenuWidget::LoginBtnClicked);
}

void UMainMenuWidget::SwitchToMainMenuWidget()
{
	if (MainSwitcher)
	{
		MainSwitcher->SetActiveWidget(MainWidgetRoot);
	}
}

void UMainMenuWidget::LoginBtnClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Logining In!"));
	if (CGameInstance && !CGameInstance->IsLoggedIn() && !CGameInstance->IsLoggingIn())
	{
		CGameInstance->ClientAccountPortalLogin();
		SwitchToWaitingWidget(FText::FromString("Logging In"));
	}
}

void UMainMenuWidget::LoginCompleted(bool bWasSuccessful, const FString& PlayerNickName, const FString& ErrorMsg)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Successful"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Failed"));
	}
	SwitchToMainMenuWidget();
}

FOnButtonClickedEvent& UMainMenuWidget::SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowCancel)
{
	MainSwitcher->SetActiveWidget(WaitingWidget);
	WaitingWidget->SetWaitInfo(WaitInfo, bAllowCancel);
	return WaitingWidget->ClearAndGetButtonClickedEvent();
}
