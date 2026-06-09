// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamSelectionWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, uint8 /*SlotId*/)

/**
 * 队伍选择界面
 */
UCLASS()
class UTeamSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnSlotClicked OnSlotClicked;
	
	void SetSlotId(uint8 NewSlotId);
	void UpdateSlotInfo(const FString& PlayerNickName);
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	class UButton* SelectButton;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* InfoText;
	
	uint8 SlotId;
	
	UFUNCTION()
	void SelectButtonClicked();
};
