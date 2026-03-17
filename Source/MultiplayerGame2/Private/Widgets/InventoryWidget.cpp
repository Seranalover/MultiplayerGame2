// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryWidget.h"

#include "InventoryItemWidget.h"
#include "Components/WrapBox.h"
#include "Inventory/InventoryComponent.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		InventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>(); //加载组件
		if (InventoryComponent)
		{
			InventoryComponent->OnItemAdded.AddUObject(this, &UInventoryWidget::ItemAdded); //订阅广播的委托
			InventoryComponent->OnItemStackCountChanged.AddUObject(this, &UInventoryWidget::ItemStackCountChanged);
			int Capacity = InventoryComponent->GetCapacity(); //装备栏数量
			ItemList->ClearChildren(); //清除子节点
			for (int i = 0; i < Capacity; ++i)
			{
				UInventoryItemWidget* NewEmptyWidget = CreateWidget<UInventoryItemWidget>(GetOwningPlayer(), ItemWidgetClass); //创建新装备格控件
				if (NewEmptyWidget)
				{
					NewEmptyWidget->SetSlotNumber(i); //设置槽位编号
					UWrapBoxSlot* NewItemSlot = ItemList->AddChildToWrapBox(NewEmptyWidget); //将新装备格加入装备栏
					NewEmptyWidget->SetPadding(FMargin(2.f)); //设置装备格内边距
					ItemWidgets.Add(NewEmptyWidget); //将新装备格加入装备栏
				}
			}
		}
	}
}

void UInventoryWidget::ItemAdded(const UInventoryItem* InventoryItem)
{
	if (!InventoryItem) return;
	
	if (UInventoryItemWidget* NextAvailableSlot = GetNextAvailableSlot())
	{
		NextAvailableSlot->UpdateInventoryItem(InventoryItem);
		PopulatedItemEntryWidgets.Add(InventoryItem->GetHandle(), NextAvailableSlot);
		if (InventoryComponent)
		{
			InventoryComponent->ItemSlotChanged(InventoryItem->GetHandle(), NextAvailableSlot->GetSlotNumber());
		}
	}
}

void UInventoryWidget::ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount)
{
	UInventoryItemWidget** FoundWidget = PopulatedItemEntryWidgets.Find(Handle);
	if (FoundWidget)
		(*FoundWidget)->UpdateStackCount();
}

UInventoryItemWidget* UInventoryWidget::GetNextAvailableSlot() const
{
	for (UInventoryItemWidget* Widget : ItemWidgets)
	{
		if (Widget->IsEmpty())
			return Widget;
	}
	return nullptr;
}
