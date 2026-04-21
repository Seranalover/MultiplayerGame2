// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerController.h"

#include "CPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/GameplayWidget.h"

//server side
void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	CPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ServerSideInit();
		CPlayerCharacter->SetGenericTeamId(TeamId);
	}
}

//client side
void ACPlayerController::AcknowledgePossession(class APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	CPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ClientSideInit();
		SpawnGameplayWidget();
	}
}

void ACPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

FGenericTeamId ACPlayerController::GetGenericTeamId() const
{
	return TeamId;
}

void ACPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPlayerController, TeamId); //同步到客户端
}

void ACPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem = 
		GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(); //本地玩家的增强输入系统
	if (InputLocalPlayerSubsystem)
	{
		InputLocalPlayerSubsystem->RemoveMappingContext(UIInputMapping);
		InputLocalPlayerSubsystem->AddMappingContext(UIInputMapping, 1); //为本地玩家控制器配置IM，设置更高的UI输入优先级
	}
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(ShopToggleInputAction, ETriggerEvent::Triggered, this, &ACPlayerController::ToggleShop);
		EnhancedInputComponent->BindAction(ToggleGameplayMenuAction, ETriggerEvent::Triggered, this, &ACPlayerController::ToggleGameplayMenu);
	}
}

//生成GUI，并添加到视口
void ACPlayerController::SpawnGameplayWidget()
{
	if (!IsLocalPlayerController()) return; //非本地玩家控制器
	
	GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
	if (GameplayWidget)
	{
		GameplayWidget->AddToViewport();
		GameplayWidget->ConfigureAbilities(CPlayerCharacter->GetAbilities());
	}
}

void ACPlayerController::ToggleShop()
{
	if (GameplayWidget)
	{
		GameplayWidget->ToggleShop();
	}
}

void ACPlayerController::ToggleGameplayMenu()
{
	if (GameplayWidget)
	{
		GameplayWidget->ToggleGameplayMenu();
	}
}
