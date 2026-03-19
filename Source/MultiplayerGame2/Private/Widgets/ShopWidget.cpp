// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ShopWidget.h"

#include "ItemTreeWidget.h"
#include "Widgets/ShopItemWidget.h"
#include "Components/TileView.h"
#include "Framework/CAssetManager.h"
#include "Inventory/InventoryComponent.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	LoadShopItems();
	ShopItemList->OnEntryWidgetGenerated().AddUObject(this, &UShopWidget::ShopItemWidgetGenerated);
	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		OwnerInventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>(); //引入库存组件
	}
}

void UShopWidget::LoadShopItems()
{
	UCAssetManager::Get().LoadShopItems(FStreamableDelegate::CreateUObject(this, &UShopWidget::ShopItemLoadFinished));
}

void UShopWidget::ShopItemLoadFinished()
{
	TArray<const UPA_ShopItem*> ShopItems;
	UCAssetManager::Get().GetLoadedShopItems(ShopItems);
	for (const UPA_ShopItem* ShopItem : ShopItems)
	{
		ShopItemList->AddItem(const_cast<UPA_ShopItem*>(ShopItem));
	}
}

void UShopWidget::ShopItemWidgetGenerated(UUserWidget& NewWidget)
{
	UShopItemWidget* ItemWidget = Cast<UShopItemWidget>(&NewWidget);
	if (ItemWidget)
	{
		if (OwnerInventoryComponent)
		{
			ItemWidget->OnItemPurchaseIssued.AddUObject(OwnerInventoryComponent, &UInventoryComponent::TryPurchase);
		}
		ItemWidget->OnShopItemSelected.AddUObject(this, &UShopWidget::ShowItemCombination); //展示合成树
		ItemsMap.Add(ItemWidget->GetShopItem(), ItemWidget);
	}
}

void UShopWidget::ShowItemCombination(const UShopItemWidget* ItemWidget)
{
	if (CombinationTree)
	{
		CombinationTree->DrawFromNode(ItemWidget); //绘制合成树
	}
}

