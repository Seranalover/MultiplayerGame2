// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UAbilitySystemComponent;
class UPA_ShopItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const UInventoryItem* /*NewItem*/); //声明委托事件，用于广播

/**
 * 库存组件类
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	FOnItemAddedDelegate OnItemAdded;
	
	// Sets default values for this component's properties
	UInventoryComponent();
	
	void TryPurchase(const UPA_ShopItem* ItemToPurchase); //尝试购买物品
	float GetGold() const; //获得金币数
	FORCEINLINE int GetCapacity() const { return Capacity; }
	void ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber); //装备插槽变更
	UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle);
	
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
	
/***********************************************************************************/
/*                                      Server                                     */
/***********************************************************************************/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Purchase(const UPA_ShopItem* ItemToPurchase); //server购买物品
	void GrantItem(const UPA_ShopItem* NewItem); //购买物品，并向client同步
	
/***********************************************************************************/
/*                                      Client                                     */
/***********************************************************************************/
private:
	UFUNCTION(Client, Reliable)
	void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* Item); //客户端同步添加购买的物品
};
