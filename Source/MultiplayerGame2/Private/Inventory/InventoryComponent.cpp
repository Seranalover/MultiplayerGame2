// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "PA_ShopItem.h"
#include "Framework/CAssetManager.h"
#include "GAS/CHeroAttributeSet.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UInventoryComponent::TryPurchase(const UPA_ShopItem* ItemToPurchase)
{
	if (!OwnerAbilitySystemComponent) return;
	
	Server_Purchase(ItemToPurchase);
}

float UInventoryComponent::GetGold() const
{
	bool bFound = false;
	if (OwnerAbilitySystemComponent)
	{
		float Gold = OwnerAbilitySystemComponent->GetGameplayAttributeValue(UCHeroAttributeSet::GetGoldAttribute(), bFound);
		if (bFound)
			return Gold;
	}
	return 0.0f;
}

void UInventoryComponent::ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber)
{
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		FoundItem->SetSlot(NewSlotNumber);
	}
}

UInventoryItem* UInventoryComponent::GetInventoryItemByHandle(const FInventoryItemHandle& Handle)
{
	UInventoryItem* const* FoundItem = InventoryMap.Find(Handle);
	if (FoundItem)
		return *FoundItem;
	return nullptr;
}

bool UInventoryComponent::IsAllSlotOccupied() const
{
	return InventoryMap.Num() >= GetCapacity();
}

UInventoryItem* UInventoryComponent::GetAvailableStackForItem(const UPA_ShopItem* Item) const
{
	if (!Item->GetIsStackable()) return nullptr;
	
	for (const TPair<FInventoryItemHandle, UInventoryItem*>& Pair : InventoryMap)
	{
		if (Pair.Value && Pair.Value->IsForItem(Item) && !Pair.Value->IsStackFull())
			return Pair.Value;
	}
	return nullptr;
}

bool UInventoryComponent::IsFullFor(const UPA_ShopItem* Item) const
{
	if (!Item) return false;
	
	if (IsAllSlotOccupied())
		return GetAvailableStackForItem(Item) == nullptr;
	
	return false;
}

void UInventoryComponent::TryActivateItem(const FInventoryItemHandle& ItemHandle)
{
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem) return;
	
	Server_ActivateItem(ItemHandle);
}

void UInventoryComponent::SellItem(const FInventoryItemHandle& ItemHandle)
{
	Server_SellItem(ItemHandle);
}

bool UInventoryComponent::FindIngredientForItem(const UPA_ShopItem* Item, TArray<UInventoryItem*>& OutIngredients,
	const TArray<const UPA_ShopItem*>& IngredientToIgnore)
{
	const FItemCollection* Ingredients = UCAssetManager::Get().GetIngredientForItem(Item);
	if (!Ingredients) return false; //未找到合成材料
	
	bool bAllFound = true;
	for (const UPA_ShopItem* Ingredient : Ingredients->GetItems())
	{
		if (IngredientToIgnore.Contains(Ingredient)) //如果是因装备已满，无法添加到装备栏的可购买合成物品
			continue;
		UInventoryItem* FoundItem = TryGetItemForShopItem(Ingredient); //在物品栏找到对应合成材料？
		if (!FoundItem)
		{
			bAllFound = false;
			break;
		}
		OutIngredients.Add(FoundItem);
	}
	return bAllFound;
}

UInventoryItem* UInventoryComponent::TryGetItemForShopItem(const UPA_ShopItem* Item) const
{
	if (!Item) return nullptr;
	
	for (const TPair<FInventoryItemHandle, UInventoryItem*>& ItemHandlePair : InventoryMap)
	{
		if (ItemHandlePair.Value && ItemHandlePair.Value->GetShopItem() == Item)
			return ItemHandlePair.Value;
	}
	return nullptr;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (OwnerAbilitySystemComponent)
		OwnerAbilitySystemComponent->AbilityCommittedCallbacks.AddUObject(this, &UInventoryComponent::AbilityCommitted);
}

void UInventoryComponent::AbilityCommitted(class UGameplayAbility* CommittedAbility)
{
	if (!CommittedAbility) return;
	
	float CooldownTimeRemaining = 0.f;
	float CooldownDuration = 0.f;
	CommittedAbility->GetCooldownTimeRemainingAndDuration(
		CommittedAbility->GetCurrentAbilitySpecHandle(),
		CommittedAbility->GetCurrentActorInfo(),
		CooldownTimeRemaining,
		CooldownDuration
	);
	
	for (TPair<FInventoryItemHandle, UInventoryItem*>& ItemHandlePair : InventoryMap)
	{
		if (!ItemHandlePair.Value) continue;
		if (ItemHandlePair.Value->IsGrantingAbility(CommittedAbility->GetClass()))
		{
			OnItemAbilityCommitted.Broadcast(ItemHandlePair.Key, CooldownDuration, CooldownTimeRemaining);
		}
	}
}

void UInventoryComponent::GrantItem(const UPA_ShopItem* NewItem)
{
	if (!GetOwner()->HasAuthority()) return;
	
	if (UInventoryItem* StackItem = GetAvailableStackForItem(NewItem))
	{
		StackItem->AddStackCount(); //堆叠数+1
		OnItemStackCountChanged.Broadcast(StackItem->GetHandle(), StackItem->GetStackCount()); //广播物品堆叠数变更
		Client_ItemStackCountChanged(StackItem->GetHandle(), StackItem->GetStackCount()); //client同步广播堆叠数变更
	}
	else
	{
		if (TryItemCombination(NewItem)) return; //成功合成高级物品，无需添加
		UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
		FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
		InventoryItem->InitItem(NewHandle, NewItem, OwnerAbilitySystemComponent);
		InventoryMap.Add(NewHandle, InventoryItem);
		OnItemAdded.Broadcast(InventoryItem); //广播事件
		// UE_LOG(LogTemp, Warning, TEXT("Server adding shop item: %s, with id: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), NewHandle.GetHandleId());
		FGameplayAbilitySpecHandle GrantedAbilitySpecHandle = InventoryItem->GetGrantedAbilitySpecHandle();
		Client_ItemAdded(NewHandle, NewItem, GrantedAbilitySpecHandle); //客户端同步购买物品
	}
}

void UInventoryComponent::Server_ActivateItem_Implementation(FInventoryItemHandle ItemHandle)
{
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem) return;
	
	InventoryItem->TryActivateGrantedAbility(); //尝试激活能力
	const UPA_ShopItem* Item = InventoryItem->GetShopItem();
	if (Item->GetIsConsumable()) 
		ConsumeItem(InventoryItem); //如果是消耗品，消耗该物品
}

bool UInventoryComponent::Server_ActivateItem_Validate(FInventoryItemHandle ItemHandle)
{
	return true;
}

void UInventoryComponent::ConsumeItem(UInventoryItem* Item)
{
	if (!GetOwner()->HasAuthority()) return;
	if (!Item) return;
	
	Item->ApplyConsumeEffect(); //应用消耗效果器
	if (!Item->ReduceStackCount()) //堆叠数减少成功？
	{
		RemoveItem(Item); //堆叠数耗尽，移除该物品
	}
	else //堆叠数未耗尽
	{
		OnItemStackCountChanged.Broadcast(Item->GetHandle(), Item->GetStackCount()); //广播堆叠数减少变更
		Client_ItemStackCountChanged(Item->GetHandle(), Item->GetStackCount()); //客户端同步堆叠数减少
	}
}

void UInventoryComponent::RemoveItem(UInventoryItem* Item)
{
	if (!GetOwner()->HasAuthority()) return;
	Item->RemoveGASModifications(); //GAS移除效果器
	OnItemRemoved.Broadcast(Item->GetHandle()); //广播移除事件
	InventoryMap.Remove(Item->GetHandle()); //移除映射
	Client_ItemRemoved(Item->GetHandle()); //client同步移除物品
}

bool UInventoryComponent::TryItemCombination(const UPA_ShopItem* NewItem)
{
	if (!GetOwner()->HasAuthority()) return false;
	
	const FItemCollection* CombinationItems = UCAssetManager::Get().GetCombinationForItem(NewItem);
	if (!CombinationItems) return false; //不存在合成路线，直接返回
	
	for (const UPA_ShopItem* CombinationItem : CombinationItems->GetItems())
	{
		TArray<UInventoryItem*> Ingredients;
		if (!FindIngredientForItem(CombinationItem, Ingredients, TArray<const UPA_ShopItem*>{NewItem}))
			continue;
		
		for (UInventoryItem* Ingredient : Ingredients) //找到合成材料后
		{
			RemoveItem(Ingredient); //移除所哟原材料
		}
		GrantItem(CombinationItem); //添加合成后物品
		return true;
	}
	return false;
}

void UInventoryComponent::Server_SellItem_Implementation(FInventoryItemHandle ItemHandle)
{
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem || !InventoryItem->IsValid()) return;
	float SellPrice = InventoryItem->GetShopItem()->GetSellPrice();
	OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, 
		SellPrice * InventoryItem->GetStackCount()); //出售物品
	RemoveItem(InventoryItem);
}

bool UInventoryComponent::Server_SellItem_Validate(FInventoryItemHandle ItemHandle)
{
	return true;
}

void UInventoryComponent::Client_ItemRemoved_Implementation(FInventoryItemHandle ItemHandle)
{
	if (GetOwner()->HasAuthority()) return;
	
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem) return;
	
	InventoryItem->RemoveGASModifications();
	OnItemRemoved.Broadcast(ItemHandle);
	InventoryMap.Remove(ItemHandle);
}

void UInventoryComponent::Client_ItemStackCountChanged_Implementation(FInventoryItemHandle Handle, int NewCount)
{
	if (GetOwner()->HasAuthority()) return;
	
	UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle);
	if (FoundItem)
	{
		FoundItem->SetStackCount(NewCount);
		OnItemStackCountChanged.Broadcast(Handle, NewCount);
	}
}

void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* Item, FGameplayAbilitySpecHandle GrantedAbilitySpecHandle)
{
	
	if (GetOwner()->HasAuthority()) return;
	
	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	InventoryItem->InitItem(AssignedHandle, Item, OwnerAbilitySystemComponent);
	InventoryItem->SetGrantedAbilitySpecHandle(GrantedAbilitySpecHandle);
	InventoryMap.Add(AssignedHandle, InventoryItem);
	OnItemAdded.Broadcast(InventoryItem); //广播事件
	// UE_LOG(LogTemp, Warning, TEXT("Client adding shop item: %s, with id: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), AssignedHandle.GetHandleId());
}

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase) return;
	
	if (GetGold() < ItemToPurchase->GetPrice()) return; //金钱不足
	
	if (!IsFullFor(ItemToPurchase)) //装备栏未满
	{
		OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice()); //购买物品，消耗金币
		GrantItem(ItemToPurchase);
		return;
	}
	//装备栏已满，且无法堆叠，但仍可以合成物品
	if (TryItemCombination(ItemToPurchase))
		OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice()); //购买物品，消耗金币

}

bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
	return true;
}



