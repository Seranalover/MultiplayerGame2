// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UAbilitySystemComponent;
class UPA_ShopItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const UInventoryItem* /*NewItem*/); //声明委托事件，用于广播
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedDelegate, const FInventoryItemHandle& /*ItemHandle*/); //声明委托事件，用于广播
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemStackCountChangedDelegate, const FInventoryItemHandle&, int /*NewCount*/); //声明委托事件，用于广播
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemAbilityCommitted, const FInventoryItemHandle&, float /*CooldownDuration*/, float /*CooldownTimeRemaining*/); //声明委托事件，用于广播

/**
 * 库存组件类
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	FOnItemAddedDelegate OnItemAdded; //新增物品事件
	FOnItemStackCountChangedDelegate OnItemStackCountChanged; //物品堆叠数变更事件
	FOnItemRemovedDelegate OnItemRemoved; //移除物品事件
	FOnItemAbilityCommitted OnItemAbilityCommitted; //物品赋予装备者能力事件
	
	// Sets default values for this component's properties
	UInventoryComponent();
	
	void TryPurchase(const UPA_ShopItem* ItemToPurchase); //尝试购买物品
	float GetGold() const; //获得金币数
	FORCEINLINE int GetCapacity() const { return Capacity; }
	void ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber); //装备插槽变更
	UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle);
	bool IsAllSlotOccupied() const; //装备栏已满？
	UInventoryItem* GetAvailableStackForItem(const UPA_ShopItem* Item) const; //获得可堆叠的装备格
	bool IsFullFor(const UPA_ShopItem* Item) const; //装备栏已满，且无法堆叠
	void TryActivateItem(const FInventoryItemHandle& ItemHandle); //尝试使用物品
	void SellItem(const FInventoryItemHandle& ItemHandle); //出售物品
	bool FindIngredientForItem(const UPA_ShopItem* Item, 
		TArray<UInventoryItem*>& OutIngredients, 
		const TArray<const UPA_ShopItem*>& IngredientToIgnore = TArray<const UPA_ShopItem*>{} /*装备栏已满时，尝试购买的合成物品，无法添加到装备栏*/
	); //是否找到所有合成材料？
	UInventoryItem* TryGetItemForShopItem(const UPA_ShopItem* Item) const; //尝试在装备栏找到对应合成材料
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int Capacity = 6; //最大装备数量
	
	UPROPERTY()	
	UAbilitySystemComponent* OwnerAbilitySystemComponent;
	
	UPROPERTY()
	TMap<FInventoryItemHandle, UInventoryItem*> InventoryMap;
	
	void AbilityCommitted(class UGameplayAbility* CommittedAbility);
	
/***********************************************************************************/
/*                                      Server                                     */
/***********************************************************************************/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Purchase(const UPA_ShopItem* ItemToPurchase); //server购买物品
	void GrantItem(const UPA_ShopItem* NewItem); //购买物品，并向client同步
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ActivateItem(FInventoryItemHandle ItemHandle); //server使用物品
	void ConsumeItem(UInventoryItem* Item); //消耗物品，减少堆叠数，不完全移除物品
	void RemoveItem(UInventoryItem* Item); //移除物品
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SellItem(FInventoryItemHandle ItemHandle); //server出售物品
	// void CheckItemCombination(const UInventoryItem* NewItem); //检查并合成物品
	bool TryItemCombination(const UPA_ShopItem* NewItem); //尝试合成物品
	
/***********************************************************************************/
/*                                      Client                                     */
/***********************************************************************************/
private:
	UFUNCTION(Client, Reliable)
	void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* Item); //客户端同步添加购买的物品
	
	UFUNCTION(Client, Reliable)
	void Client_ItemStackCountChanged(FInventoryItemHandle Handle, int NewCount); //client响应物品堆叠数变更
	
	UFUNCTION(Client, Reliable)
	void Client_ItemRemoved(FInventoryItemHandle ItemHandle); //client响应移除物品
};
