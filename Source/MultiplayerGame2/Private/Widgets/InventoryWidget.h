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
	
	TArray<UInventoryItemWidget*> ItemWidgets; //装备格控件
	TMap<FInventoryItemHandle, UInventoryItemWidget*> PopulatedItemEntryWidgets; //已装备的物品映射信息集合
	
	void ItemAdded(const UInventoryItem* InventoryItem); //订阅新增物品事件委托
	void ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount); //订阅物品堆叠数变更事件委托
	UInventoryItemWidget* GetNextAvailableSlot() const; //获得下一个空闲槽位
	void HandleItemDragDrop(UInventoryItemWidget* DestinationWidget, UInventoryItemWidget* SourceWidget); //拖拽交换两个控件位置
	void ItemRemoved(const FInventoryItemHandle& ItemHandle); //移除物品
};
