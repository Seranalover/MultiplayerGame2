// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/ItemWidget.h"
#include "ShopItemWidget.generated.h"

class UPA_ShopItem;
class UShopItemWidget;

//声明委托
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssued, const UPA_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);

/**
 * 商店物品控件类
 * 代表单个售卖的商店物品控件
 */
UCLASS()
class UShopItemWidget : public UItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	FOnItemPurchaseIssued OnItemPurchaseIssued; //购买委托事件
	FOnShopItemSelected OnShopItemSelected; //选中物品委托事件
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }
	
private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;
	
	virtual void RightButtonClicked() override; //点击右键，确认购买，广播委托
	virtual void LeftButtonClicked() override; //点击左键，选中物品，广播委托
};
