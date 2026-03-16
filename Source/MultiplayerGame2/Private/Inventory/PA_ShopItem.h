// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_ShopItem.generated.h"

/**
 * 物品结构体
 */
class UPA_ShopItem;
USTRUCT(BlueprintType)
struct FItemCollection
{
	GENERATED_BODY()
public:
	FItemCollection();
	FItemCollection(const TArray<const UPA_ShopItem*>& InItems);
	void AddItem(const UPA_ShopItem* NewItem, bool bAddUnique = false);
	bool ContainsItem(const UPA_ShopItem* Item) const;
	const TArray<const UPA_ShopItem*>& GetItems() const;
private:
	TArray<const UPA_ShopItem*> Items;
};

/**
 * 商店物品定义类
 * 数据资产，纯数据存储
 */
UCLASS()
class UPA_ShopItem : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetShopItemAssetType();
	
	UTexture2D* GetIcon() const;
	FText GetItemName() const { return ItemName; }
	FText GetItemDescription() const { return ItemDescription; }
	float GetPrice() const { return Price; }
	float GetSellPrice() const {return Price / 2.f; }
	TSubclassOf<class UGameplayEffect> GetEquippedEffect() const { return EquippedEffect; }
	TSubclassOf<class UGameplayEffect> GetConsumableEffect() const { return ConsumableEffect; }
	TSubclassOf<class UGameplayAbility> GetGrantedAbility() const { return GrantedAbility; }
	bool IsStackable() const { return bIsStackable; }
	bool IsConsumable() const { return bIsConsumable; }
	int GetMaxStackCount() const { return MaxStackCount; }
	const TArray<TSoftObjectPtr<UPA_ShopItem>>& GetIngredients() const { return IngredientItems; }
	
private:
	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	TSoftObjectPtr<UTexture2D> Icon; //软指针，仅在需要时加载
	
	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	float Price;
	
	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	FText ItemName;
	
	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	FText ItemDescription;
	
	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	bool bIsConsumable; //是否消耗品
	
	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	TSubclassOf<class UGameplayEffect> EquippedEffect; //装备效果
	
	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	TSubclassOf<class UGameplayEffect> ConsumableEffect; //消耗效果
	
	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	TSubclassOf<class UGameplayAbility> GrantedAbility; //赋予能力
	
	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	bool bIsStackable = false; //物品是否可堆叠
	
	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	int MaxStackCount = 5;
	
	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	TArray<TSoftObjectPtr<UPA_ShopItem>> IngredientItems; //合成材料
};
