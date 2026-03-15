// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Inventory/PA_ShopItem.h"
#include "Widgets/ItemWidget.h"
#include "ShopItemWidget.generated.h"

/**
 * 商店物品控件类
 * 代表单个售卖的商店物品控件
 */
UCLASS()
class UShopItemWidget : public UItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }
	
private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;
};
