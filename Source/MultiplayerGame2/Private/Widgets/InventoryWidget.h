// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryItem.h"
#include "InventoryWidget.generated.h"

class UInventoryItemWidget;
/**
 * 库存/装备栏 控件类
 */
UCLASS()
class UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct();
	
private:
	UPROPERTY(meta=(BindWidget))
	class UWrapBox* ItemList;
	
	UPROPERTY(EditDefaultsOnly, Category="")
	TSubclassOf<UInventoryItemWidget> ItemWidgetClass;
	
	UPROPERTY()
	class UInventoryComponent* InventoryComponent;
	
	TArray<UInventoryItemWidget*> ItemWidgets;
	TMap<FInventoryItemHandle, UInventoryItemWidget*> PopulatedItemEntryWidgets;
	
	void ItemAdded(const UInventoryItem* InventoryItem);
	UInventoryItemWidget* GetNextAvailableSlot() const; //获得下一个空闲槽位
	
};
