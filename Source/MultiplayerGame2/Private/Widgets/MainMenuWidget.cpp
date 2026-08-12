// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenuWidget.h"
#include "Framework/CGameInstance.h"
#include "Components/Button.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CGameInstance = GetGameInstance<UCGameInstance>();
	if (CGameInstance)
	{
		
	}
	
	LoginButton->OnClicked.AddDynamic(this, &UMainMenuWidget::LoginBtnClicked);
}

void UMainMenuWidget::LoginBtnClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Logining In!"));
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
}
