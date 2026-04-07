// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryItemWidget.h"

#include "InventoryItemDragDropOp.h"
#include "Components/Image.h"
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
	UnbindCanCastAbilityDelegate();
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
	ClearCooldown(); //清除冷却时间
	if (InventoryItem->IsGrantedAnyAbility())
	{
		UpdateCanCastDisplay(InventoryItem->CanCastAbility());
		float AbilityCooldownRemaining = InventoryItem->GetAbilityCooldownTimeRemaining();
		float AbilityCooldownDuration = InventoryItem->GetAbilityCooldownDuration();
		if (AbilityCooldownRemaining > 0) //剩余冷却时间大于0，说明物品技能处于冷却
			StartCooldown(AbilityCooldownDuration, AbilityCooldownRemaining); //使同名的其他物品技能也进入冷却
		CooldownDurationText->SetVisibility(AbilityCooldownDuration == 0.f ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		CooldownDurationText->SetText(FText::AsNumber(AbilityCooldownDuration));
		float AbilityCost = InventoryItem->GetAbilityManaCost();
		ManaCostText->SetVisibility(AbilityCost == 0.f ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		ManaCostText->SetText(FText::AsNumber(AbilityCost));
		BindCanCastAbilityDelegate();
	}
	else
	{
		UpdateCanCastDisplay(true);
		ManaCostText->SetVisibility(ESlateVisibility::Hidden);
		CooldownDurationText->SetVisibility(ESlateVisibility::Hidden);
		CooldownCountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryItemWidget::EmptySlot()
{
	ClearCooldown();
	UnbindCanCastAbilityDelegate();
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

void UInventoryItemWidget::UpdateCanCastDisplay(bool bCanCast)
{
	GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(CanCastDynamicMaterialParamName, bCanCast ? 1.0f : 0.0f);
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

void UInventoryItemWidget::StartCooldown(float Duration, float TimeRemaining)
{
	CooldownRemaining = TimeRemaining;
	CooldownDuration = Duration;
	GetWorld()->GetTimerManager().SetTimer(CooldownDurationTimerHandle, this, &UInventoryItemWidget::CooldownFinished, CooldownRemaining);
	GetWorld()->GetTimerManager().SetTimer(CooldownUpdateTimerHandle, this, &UInventoryItemWidget::UpdateCooldown, CooldownUpdateInterval, true);
	CooldownCountText->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryItemWidget::CooldownFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(CooldownUpdateTimerHandle);
	CooldownCountText->SetVisibility(ESlateVisibility::Hidden);
	if (GetItemIcon())
		GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(CooldownAmtDynamicMaterialParamName, 1.f); //设置动态材质-百分比参数为 1%，代表计时器关闭
}

void UInventoryItemWidget::UpdateCooldown()
{
	CooldownRemaining -= CooldownUpdateInterval;
	float CooldownAmt = 1.f - CooldownRemaining / CooldownDuration;
	CooldownFormattingOptions.MaximumFractionalDigits = CooldownRemaining > 1.f ? 0 : 2;
	CooldownCountText->SetText(FText::AsNumber(CooldownRemaining, &CooldownFormattingOptions));
	if (GetItemIcon())
		GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(CooldownAmtDynamicMaterialParamName, CooldownAmt);
}

void UInventoryItemWidget::ClearCooldown()
{
	CooldownFinished();
}

void UInventoryItemWidget::SetIcon(UTexture2D* IconTexture)
{
	if (GetItemIcon())
	{
		GetItemIcon()->GetDynamicMaterial()->SetTextureParameterValue(IconTextureDynamicMaterialParamName, IconTexture);
		return;
	}
	Super::SetIcon(IconTexture);
}

void UInventoryItemWidget::BindCanCastAbilityDelegate()
{
	if (InventoryItem)
		const_cast<UInventoryItem*>(InventoryItem)->OnAbilityCanCastUpdatedDelegate.AddUObject(this, &UInventoryItemWidget::UpdateCanCastDisplay);
}

void UInventoryItemWidget::UnbindCanCastAbilityDelegate()
{
	if (InventoryItem)
		const_cast<UInventoryItem*>(InventoryItem)->OnAbilityCanCastUpdatedDelegate.RemoveAll(this);
}
