// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/PA_ShopItem.h"

#include "Abilities/GameplayAbility.h"

FItemCollection::FItemCollection()
	:Items{}
{
}

FItemCollection::FItemCollection(const TArray<const UPA_ShopItem*>& InItems)
	:Items{InItems}
{
}

void FItemCollection::AddItem(const UPA_ShopItem* NewItem, bool bAddUnique)
{
	if (bAddUnique && ContainsItem(NewItem))
		return;
	
	Items.Add(NewItem);
}

bool FItemCollection::ContainsItem(const UPA_ShopItem* Item) const
{
	return Items.Contains(Item);
}

const TArray<const UPA_ShopItem*>& FItemCollection::GetItems() const
{
	return Items;
}

FPrimaryAssetId UPA_ShopItem::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetShopItemAssetType(), GetFName());
}

FPrimaryAssetType UPA_ShopItem::GetShopItemAssetType()
{
	return FPrimaryAssetType("ShopItem");
}

UTexture2D* UPA_ShopItem::GetIcon() const
{
	return Icon.LoadSynchronous(); //加载图标
}

class UGameplayAbility* UPA_ShopItem::GetGrantedAbilityCDO() const
{
	if (GrantedAbility)
		return Cast<UGameplayAbility>(GrantedAbility->GetDefaultObject());
	return nullptr;
}
