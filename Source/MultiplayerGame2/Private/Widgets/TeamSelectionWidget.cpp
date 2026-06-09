// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/TeamSelectionWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTeamSelectionWidget::SetSlotId(uint8 NewSlotId)
{
	SlotId = NewSlotId;
}

void UTeamSelectionWidget::UpdateSlotInfo(const FString& PlayerNickName)
{
	InfoText->SetText(FText::FromString(PlayerNickName));
}

void UTeamSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SelectButton->OnClicked.AddDynamic(this, &UTeamSelectionWidget::SelectButtonClicked);
}

void UTeamSelectionWidget::SelectButtonClicked()
{
	OnSlotClicked.Broadcast(SlotId);
}
