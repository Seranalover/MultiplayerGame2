// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "InventoryContextMenuWidget.generated.h"

/**
 * 库存物品上下文菜单控件
 */
UCLASS()
class UInventoryContextMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnButtonClickedEvent& GetSellButtonClickedEvent() const; //用于监听sell按钮的点击
	FOnButtonClickedEvent& GetUseButtonClickedEvent() const; //用于监听use按钮的点击
	
private:
	UPROPERTY(meta=(BindWidget))
	UButton* UseButton;
	UPROPERTY(meta=(BindWidget))
	UButton* SellButton;
};
