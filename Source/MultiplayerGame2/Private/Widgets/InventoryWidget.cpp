// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryWidget.h"

#include "InventoryItemWidget.h"
#include "Components/WrapBox.h"
#include "Inventory/InventoryComponent.h"
#include "Widgets/InventoryContextMenuWidget.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		InventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>(); //加载组件
		if (InventoryComponent)
		{
			InventoryComponent->OnItemAdded.AddUObject(this, &UInventoryWidget::ItemAdded); //订阅新增物品广播委托
			InventoryComponent->OnItemRemoved.AddUObject(this, &UInventoryWidget::ItemRemoved); //订阅删除物品广播委托
			InventoryComponent->OnItemStackCountChanged.AddUObject(this, &UInventoryWidget::ItemStackCountChanged); //订阅物品堆叠数变更广播委托
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
					
					NewEmptyWidget->OnInventoryItemDropped.AddUObject(this, &UInventoryWidget::HandleItemDragDrop); //订阅拖拽事件委托
					NewEmptyWidget->OnLeftButtonClick.AddUObject(InventoryComponent, &UInventoryComponent::TryActivateItem); //订阅左键点击使用物品事件委托
					NewEmptyWidget->OnRightButtonClick.AddUObject(this, &UInventoryWidget::ToggleContextMenu);
				}
			}
			SpawnContextMenu();
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

void UInventoryWidget::HandleItemDragDrop(UInventoryItemWidget* DestinationWidget, UInventoryItemWidget* SourceWidget)
{
	//从控件中获得物品对象
	const UInventoryItem* SourceItem = SourceWidget->GetInventoryItem(); 
	const UInventoryItem* DestinationItem = DestinationWidget->GetInventoryItem();
	
	//交换物品对象
	DestinationWidget->UpdateInventoryItem(SourceItem); 
	SourceWidget->UpdateInventoryItem(DestinationItem);
	
	//更新目标控件映射信息
	PopulatedItemEntryWidgets[DestinationWidget->GetInventoryItemHandle()] = DestinationWidget;
	if (InventoryComponent)
	{
		InventoryComponent->ItemSlotChanged(DestinationWidget->GetInventoryItemHandle(), DestinationWidget->GetSlotNumber());
	}
	
	//拖拽后源控件不为空，更新源控件映射信息，拖拽后源控件为空，不做处理
	if (!SourceWidget->IsEmpty())
	{
		PopulatedItemEntryWidgets[SourceWidget->GetInventoryItemHandle()] = SourceWidget;
		if (InventoryComponent)
			InventoryComponent->ItemSlotChanged(SourceWidget->GetInventoryItemHandle(), SourceWidget->GetSlotNumber());
	}
}

void UInventoryWidget::ItemRemoved(const FInventoryItemHandle& ItemHandle)
{
	UInventoryItemWidget** FoundWidget = PopulatedItemEntryWidgets.Find(ItemHandle);
	if (FoundWidget && *FoundWidget)
	{
		(*FoundWidget)->EmptySlot();
		PopulatedItemEntryWidgets.Remove(ItemHandle);
	}
}

void UInventoryWidget::SpawnContextMenu()
{
	if (!ContextMenuWidgetClass) return;
	
	ContextMenuWidget = CreateWidget<UInventoryContextMenuWidget>(this, ContextMenuWidgetClass);
	if (ContextMenuWidget)
	{
		ContextMenuWidget->GetSellButtonClickedEvent().AddDynamic(this, &UInventoryWidget::SellFocusedItem); //监听点击sell按钮事件，绑定到SellFocusedItem函数
		ContextMenuWidget->GetUseButtonClickedEvent().AddDynamic(this, &UInventoryWidget::UseFocusedItem);
		ContextMenuWidget->AddToViewport(1);
		SetContextMenuVisible(false);
	}
}

void UInventoryWidget::SellFocusedItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Selling Item"));
}

void UInventoryWidget::UseFocusedItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Using Item"));
}

void UInventoryWidget::SetContextMenuVisible(bool bVisible)
{
	if (ContextMenuWidget)
		ContextMenuWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UInventoryWidget::ToggleContextMenu(const FInventoryItemHandle& ItemHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("Toggling ContextMenu"));
}