// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ShopItemWidget.h"

void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	ShopItem = Cast<UPA_ShopItem>(ListItemObject);
	if (!ShopItem) return;
	
	SetIcon(ShopItem->GetIcon());
	SetToolTipWidget(ShopItem);
}

void UShopItemWidget::RightButtonClicked()
{
	OnItemPurchaseIssued.Broadcast(GetShopItem()); //广播issued事件
}

void UShopItemWidget::LeftButtonClicked()
{
	OnShopItemSelected.Broadcast(this); //广播selected事件
}
