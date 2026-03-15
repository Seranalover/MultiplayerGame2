// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShopItemWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/TileView.h"
#include "Inventory/PA_ShopItem.h"
#include "ShopWidget.generated.h"

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
	
	void LoadShopItems();
	void ShopItemLoadFinished();
	void ShopItemWidgetGenerated(UUserWidget& NewWidget);
};
