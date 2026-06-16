// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CAssetManager.h"

#include "Character/PA_CharacterDefination.h"

UCAssetManager& UCAssetManager::Get()
{
	UCAssetManager* Singleton = Cast<UCAssetManager>(GEngine->AssetManager.Get());
	if (Singleton) return *Singleton;
	
	UE_LOG(LogLoad, Fatal, TEXT("Asset Manager Singleton is NULL"));
	return *NewObject<UCAssetManager>();
}

void UCAssetManager::LoadCharacterDefinations(const FStreamableDelegate& LoadFinishedCallback)
{
	LoadPrimaryAssetsWithType(UPA_CharacterDefination::GetCharacterDefinationAssetType(), TArray<FName>(), LoadFinishedCallback);
}

bool UCAssetManager::GetLoadedCharacterDefinaions(TArray<UPA_CharacterDefination*>& LoadedCharacterDefinations) const
{
	TArray<UObject*> LoadedObjects;
	bool bLoaded = GetPrimaryAssetObjectList(UPA_CharacterDefination::GetCharacterDefinationAssetType(), LoadedObjects);
	if (bLoaded)
	{
		for (UObject* LoadedObject : LoadedObjects)
		{
			LoadedCharacterDefinations.Add(Cast<UPA_CharacterDefination>(LoadedObject));
		}
	}
	return bLoaded;
}

void UCAssetManager::LoadShopItems(const FStreamableDelegate& LoadFinishedCallback)
{
	LoadPrimaryAssetsWithType(UPA_ShopItem::GetShopItemAssetType(), TArray<FName>(), 
		FStreamableDelegate::CreateUObject(this, &UCAssetManager::ShopItemLoadFinished, LoadFinishedCallback));
}

bool UCAssetManager::GetLoadedShopItems(TArray<const UPA_ShopItem*>& OutItems) const
{
	TArray<UObject*> LoadedItems;
	bool bLoaded = GetPrimaryAssetObjectList(UPA_ShopItem::GetShopItemAssetType(), LoadedItems);
	if (bLoaded)
	{
		for (UObject* Object : LoadedItems)
		{
			OutItems.Add(Cast<UPA_ShopItem>(Object));
		}
	}
	return bLoaded;
}

const FItemCollection* UCAssetManager::GetCombinationForItem(const UPA_ShopItem* Item) const
{
	return CombinationMap.Find(Item);
}

const FItemCollection* UCAssetManager::GetIngredientForItem(const UPA_ShopItem* Item) const
{
	return IngredientMap.Find(Item);
}

void UCAssetManager::ShopItemLoadFinished(FStreamableDelegate Callback)
{
	Callback.ExecuteIfBound();
	BuildItemMaps();
}

void UCAssetManager::BuildItemMaps()
{
	TArray<const UPA_ShopItem*> LoadedItems;
	if (GetLoadedShopItems(LoadedItems))
	{
		for (const UPA_ShopItem* Item : LoadedItems)
		{
			if (Item->GetIngredients().Num() == 0)
				continue;
			
			TArray<const UPA_ShopItem*> Items;
			for (const TSoftObjectPtr<UPA_ShopItem>& Ingredient : Item->GetIngredients())
			{
				UPA_ShopItem* IngredientItem = Ingredient.LoadSynchronous();
				Items.Add(IngredientItem);
				AddToCombinationMap(IngredientItem, Item);
			}
			
			IngredientMap.Add(Item, FItemCollection{Items});
		}
	}
}

void UCAssetManager::AddToCombinationMap(const UPA_ShopItem* IngredientItem, const UPA_ShopItem* CombinationItem)
{
	FItemCollection* Combinations = CombinationMap.Find(IngredientItem);
	if (Combinations)
	{
		if (!Combinations->ContainsItem(CombinationItem))
			Combinations->AddItem(CombinationItem);
	}
	else
	{
		CombinationMap.Add(IngredientItem, FItemCollection{TArray<const UPA_ShopItem*>{CombinationItem}});
	}
}
