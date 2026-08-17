// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenuWidget.h"

#include "SessionEntryWidget.h"
#include "Framework/CGameInstance.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "Network/CNetStatics.h"
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
		CGameInstance->OnJoinSessionFailed.AddUObject(this, &UMainMenuWidget::JoinSessionFailed);
		CGameInstance->OnGlobalSessionSearchCompleted.AddUObject(this, &UMainMenuWidget::UpdateLobbyList);
		CGameInstance->StartGlobalSessionSearch();
	}
	LoginButton->OnClicked.AddDynamic(this, &UMainMenuWidget::LoginBtnClicked);
	
	CreateSessionBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::CreateSessionBtnClicked);
	CreateSessionBtn->SetIsEnabled(false);
	NewSessionNameText->OnTextChanged.AddDynamic(this, &UMainMenuWidget::NewSessionNameTextChanged);
	
	JoinSessionBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::JoinSessionBtnClicked);
	JoinSessionBtn->SetIsEnabled(false);
}

void UMainMenuWidget::SwitchToMainMenuWidget()
{
	if (MainSwitcher)
	{
		MainSwitcher->SetActiveWidget(MainWidgetRoot);
	}
}

void UMainMenuWidget::CreateSessionBtnClicked()
{
	if (CGameInstance && CGameInstance->IsLoggedIn())
	{
		CGameInstance->RequestCreateAndJoinSession(FName(NewSessionNameText->GetText().ToString()));
		SwitchToWaitingWidget(FText::FromString("Creating Lobby"), true).AddDynamic(this, &UMainMenuWidget::CancelSessionCreation);
	}
}

void UMainMenuWidget::NewSessionNameTextChanged(const FText& NewText)
{
	CreateSessionBtn->SetIsEnabled(!NewText.IsEmpty());
}

void UMainMenuWidget::CancelSessionCreation()
{
	if (CGameInstance)
	{
		CGameInstance->CancelSessionCreation();
	}
	SwitchToMainMenuWidget();
}

void UMainMenuWidget::JoinSessionBtnClicked()
{
	if (CGameInstance && !CurrentSelectedSessionId.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to join Session with id: %s"), *CurrentSelectedSessionId);
		if (CGameInstance->JoinSessionWithId(CurrentSelectedSessionId))
		{
			SwitchToWaitingWidget(FText::FromString("Joining"), false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't join session, no session seleted"));
	}
}

void UMainMenuWidget::JoinSessionFailed()
{
	SwitchToMainMenuWidget();
}

void UMainMenuWidget::UpdateLobbyList(const TArray<FOnlineSessionSearchResult>& SearchResults)
{
	UE_LOG(LogTemp, Warning, TEXT("Updating Session Search Results"));
	SessionScrollBox->ClearChildren();
	
	bool bCurrentSelectedSessionValid = false;
	for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
	{
		USessionEntryWidget* NewSessionEntryWidget = CreateWidget<USessionEntryWidget>(GetOwningPlayer(), SessionEntryWidgetClass);
		if (NewSessionEntryWidget)
		{
			FString SessionName = "Name_None";
			SearchResult.Session.SessionSettings.Get<FString>(UCNetStatics::GetSessionNameKey(), SessionName);
			
			FString SessionId = SearchResult.Session.GetSessionIdStr();
			NewSessionEntryWidget->InitializeEntry(SessionName, SessionId);
			
			NewSessionEntryWidget->OnSessionEntrySelected.AddUObject(this, &UMainMenuWidget::SessionEntrySelected);
			SessionScrollBox->AddChild(NewSessionEntryWidget);
			if (CurrentSelectedSessionId == SessionId)
			{
				bCurrentSelectedSessionValid = true;
			}
		}
	}
	CurrentSelectedSessionId = bCurrentSelectedSessionValid ? CurrentSelectedSessionId : "";
	JoinSessionBtn->SetIsEnabled(bCurrentSelectedSessionValid);
}

void UMainMenuWidget::SessionEntrySelected(const FString& SelectedEntryId)
{
	CurrentSelectedSessionId = SelectedEntryId;
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
