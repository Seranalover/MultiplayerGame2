// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryItemWidget.h"

#include "InventoryItemDragDropOp.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem.h"

void UInventoryItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EmptySlot();
}

bool UInventoryItemWidget::IsEmpty() const
{
	return !InventoryItem || !(InventoryItem->IsValid());
}

void UInventoryItemWidget::UpdateInventoryItem(const UInventoryItem* Item)
{
	InventoryItem = Item;
	if (!InventoryItem || !InventoryItem->IsValid() || InventoryItem->GetStackCount() <= 0)
	{
		EmptySlot();
		return;
	}
	SetIcon(Item->GetShopItem()->GetIcon());
	UItemToolTip* ToolTip = SetToolTipWidget(InventoryItem->GetShopItem());
	if (ToolTip)
	{
		ToolTip->SetPrice(InventoryItem->GetShopItem()->GetSellPrice());
	}
	if (InventoryItem->GetShopItem()->GetIsStackable())
	{
		StackCountText->SetVisibility(ESlateVisibility::Visible);
		UpdateStackCount();
	}
	else
	{
		StackCountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryItemWidget::EmptySlot()
{
	InventoryItem = nullptr;
	SetIcon(EmptyTexture);
	SetToolTip(nullptr);
	
	StackCountText->SetVisibility(ESlateVisibility::Hidden);
	ManaCostText->SetVisibility(ESlateVisibility::Hidden);
	CooldownCountText->SetVisibility(ESlateVisibility::Hidden);
	CooldownDurationText->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryItemWidget::SetSlotNumber(int NewSlotNumber)
{
	SlotNumber = NewSlotNumber;
}

void UInventoryItemWidget::UpdateStackCount()
{
	if (InventoryItem)
	{
		StackCountText->SetText(FText::AsNumber(InventoryItem->GetStackCount()));
	}
}

UTexture2D* UInventoryItemWidget::GetIconTexture() const
{
	if (InventoryItem && InventoryItem->GetShopItem())
		return InventoryItem->GetShopItem()->GetIcon();
	return nullptr;
}

FInventoryItemHandle UInventoryItemWidget::GetInventoryItemHandle() const
{
	if (!IsEmpty())
		return InventoryItem->GetHandle();
	return FInventoryItemHandle::InvalidHandle();
}

void UInventoryItemWidget::RightButtonClicked()
{
	if (!IsEmpty())
		OnRightButtonClick.Broadcast(GetInventoryItemHandle());
}

void UInventoryItemWidget::LeftButtonClicked()
{
	if (!IsEmpty())
		OnLeftButtonClick.Broadcast(GetInventoryItemHandle());
}

void UInventoryItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (!IsEmpty() && DragDropOpClass)
	{
		UInventoryItemDragDropOp* DragDropOp = NewObject<UInventoryItemDragDropOp>(this, DragDropOpClass); //定义拖拽操作
		if (DragDropOp)
		{
			DragDropOp->SetDraggedItem(this); //配置拖拽对象
			OutOperation = DragDropOp;
		}
	}
}

bool UInventoryItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (UInventoryItemWidget* OtherWidget = Cast<UInventoryItemWidget>(InOperation->Payload))
	{
		if (OtherWidget && !OtherWidget->IsEmpty()) //拖拽的目标位置不为空，存在其他对象
		{
			OnInventoryItemDropped.Broadcast(this, OtherWidget); //广播拖拽事件
			return true;
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
