// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "WaitingWidget.generated.h"

/**
 * 等待界面类
 */
UCLASS()
class UWaitingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	FOnButtonClickedEvent& ClearAndGetButtonClickedEvent(); //清除状态并绑定按钮点击事件
	void SetWaitInfo(const FText& WaitInfo, bool bAllowCancel = false);
	
private:
	UPROPERTY(meta=(BindWidget))
	UButton* CancelButton;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* WaitInfoText;
};
