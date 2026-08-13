// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/WaitingWidget.h"

#include "Components/TextBlock.h"


void UWaitingWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FOnButtonClickedEvent& UWaitingWidget::ClearAndGetButtonClickedEvent()
{
	CancelButton->OnClicked.Clear();
	return CancelButton->OnClicked;
}

void UWaitingWidget::SetWaitInfo(const FText& WaitInfo, bool bAllowCancel)
{
	if (CancelButton)
	{
		CancelButton->SetVisibility(bAllowCancel ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if (WaitInfoText)
	{
		WaitInfoText->SetText(WaitInfo);
	}
}
