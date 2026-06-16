// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Inventory/PA_ShopItem.h"
#include "CAssetManager.generated.h"

class UPA_CharacterDefination;
/**
 * 资产管理器类
 */
UCLASS()
class UCAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	static UCAssetManager& Get();
	void LoadCharacterDefinations(const FStreamableDelegate& LoadFinishedCallback); //加载角色
	bool GetLoadedCharacterDefinaions(TArray<UPA_CharacterDefination*>& LoadedCharacterDefinations) const; //是否加载角色
	
	void LoadShopItems(const FStreamableDelegate& LoadFinishedCallback); //加载商店物品
	bool GetLoadedShopItems(TArray<const UPA_ShopItem*>& OutItems) const; //是否加载到商店物品
	const FItemCollection* GetCombinationForItem(const UPA_ShopItem* Item) const; //获得物品合成路线
	const FItemCollection* GetIngredientForItem(const UPA_ShopItem* Item) const; //获得物品分解路线
	
private:
	void ShopItemLoadFinished(FStreamableDelegate Callback); //商店物品加载完成时调用
	void BuildItemMaps(); //创建合称物映射集
	auto AddToCombinationMap(const UPA_ShopItem* IngredientItem, const UPA_ShopItem* CombinationItem) -> void; //添加到映射集
	
	UPROPERTY()
	TMap<const UPA_ShopItem*, FItemCollection> CombinationMap; //合成物品
	UPROPERTY()
	TMap<const UPA_ShopItem*, FItemCollection> IngredientMap; //合成材料
};
