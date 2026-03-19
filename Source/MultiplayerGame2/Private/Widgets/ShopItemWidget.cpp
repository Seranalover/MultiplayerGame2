// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ShopItemWidget.h"

#include "Components/ListView.h"
#include "Framework/CAssetManager.h"

void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	InitWithShopItem(Cast<UPA_ShopItem>(ListItemObject));
	ParentListView = Cast<UListView>(IUserListEntry::GetOwningListView());
}

UUserWidget* UShopItemWidget::GetWidget()
{
	UShopItemWidget* Copy = CreateWidget<UShopItemWidget>(GetOwningPlayer(), GetClass()); //创建当前商品控件的副本对象
	Copy->CopyFromOther(this); //复制对象内容
	return Copy;
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetInputs() const
{
	const FItemCollection* Collection = UCAssetManager::Get().GetCombinationForItem(GetShopItem());
	if (Collection)
		return ItemsToInterfaces(Collection->GetItems());
	return TArray<const ITreeNodeInterface*>{};
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetOutputs() const
{
	const FItemCollection* Collection = UCAssetManager::Get().GetIngredientForItem(GetShopItem());
	if (Collection)
		return ItemsToInterfaces(Collection->GetItems());
	return TArray<const ITreeNodeInterface*>{};
}

const UObject* UShopItemWidget::GetItemObject() const
{
	return ShopItem;
}

void UShopItemWidget::InitWithShopItem(const UPA_ShopItem* NewShopItem)
{
	ShopItem = NewShopItem;
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

void UShopItemWidget::CopyFromOther(const UShopItemWidget* OtherWidget)
{
	OnItemPurchaseIssued = OtherWidget->OnItemPurchaseIssued;
	OnShopItemSelected = OtherWidget->OnShopItemSelected;
	InitWithShopItem(OtherWidget->GetShopItem());
	ParentListView = OtherWidget->ParentListView;
}

TArray<const ITreeNodeInterface*> UShopItemWidget::ItemsToInterfaces(const TArray<const UPA_ShopItem*>& Items) const
{
	TArray<const ITreeNodeInterface*> ReturnInterfaces;
	if (!ParentListView) return  ReturnInterfaces;
	
	for (const UPA_ShopItem* Item : Items)
	{
		const UShopItemWidget* ItemWidget = ParentListView->GetEntryWidgetFromItem<UShopItemWidget>(Item);
		if (ItemWidget)
			ReturnInterfaces.Add(ItemWidget);
	}
	return ReturnInterfaces;
}
