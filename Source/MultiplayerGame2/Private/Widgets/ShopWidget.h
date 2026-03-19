// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

class UPA_ShopItem;
class UTileView;
class UInventoryComponent;
class UShopItemWidget;

/**
 * 商店页面控件类
 */
UCLASS()
class UShopWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	UTileView* ShopItemList; //shop items的容器
	
	UPROPERTY()
	TMap<const UPA_ShopItem*, const UShopItemWidget*> ItemsMap;
	
	UPROPERTY()
	UInventoryComponent* OwnerInventoryComponent;
	
	UPROPERTY(meta=(BindWidget))
	class UItemTreeWidget* CombinationTree; //装备合成树
	
	void LoadShopItems(); //资源管理器加载商店物品
	void ShopItemLoadFinished(); //加载完毕
	void ShopItemWidgetGenerated(UUserWidget& NewWidget); //为商店物品生成控件
	void ShowItemCombination(const UShopItemWidget* ItemWidget); //展示物品合成树
};
