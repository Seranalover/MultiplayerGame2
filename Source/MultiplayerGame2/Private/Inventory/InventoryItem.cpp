// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItem.h"

#include "AbilitySystemComponent.h"
#include "PA_ShopItem.h"

FInventoryItemHandle::FInventoryItemHandle()
	:HandleId(GetInvalidId()) //确保只能通过工厂函数来获得有效的实例
{
}

FInventoryItemHandle::FInventoryItemHandle(uint32 HandleId)
	:HandleId(HandleId)
{
}

FInventoryItemHandle FInventoryItemHandle::InvalidHandle()
{
	static FInventoryItemHandle InvalidHandle = FInventoryItemHandle();
	return InvalidHandle;
}

FInventoryItemHandle FInventoryItemHandle::CreateHandle()
{
	return FInventoryItemHandle(GenerateNextId());
}

bool FInventoryItemHandle::IsValid() const
{
	return HandleId != GetInvalidId();
}

uint32 FInventoryItemHandle::GenerateNextId()
{
	static uint32 StaticId = 1;
	return StaticId++;
}

uint32 FInventoryItemHandle::GetInvalidId()
{
	return 0;
}

bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs)
{
	return Lhs.GetHandleId() == Rhs.GetHandleId();
}

uint32 GetTypeHash(const FInventoryItemHandle& Key)
{
	return Key.GetHandleId();
}

UInventoryItem::UInventoryItem()
	:StackCount{1}
{
}

void UInventoryItem::InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem)
{
	Handle = NewHandle;
	ShopItem = NewShopItem;
}

void UInventoryItem::ApplyGASModifications(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!GetShopItem() || !AbilitySystemComponent) return;
	
	if (!AbilitySystemComponent->GetOwner() || !AbilitySystemComponent->GetOwner()->HasAuthority()) return;
	
	//应用effect
	TSubclassOf<UGameplayEffect> EquipEffect = GetShopItem()->GetEquippedEffect();
	if (EquipEffect)
	{
		AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(EquipEffect, 1, AbilitySystemComponent->MakeEffectContext());
	}
	
	//应用ability
	TSubclassOf<UGameplayAbility> GrantedAbility = GetShopItem()->GetGrantedAbility();
	if (GrantedAbility)
	{
		const FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromClass(GrantedAbility);
		if (FoundSpec) //能力已被赋予
		{
			GrantedAbilitySpecHandle = FoundSpec->Handle;
		}
		else //能力未被赋予，赋予能力
		{
			GrantedAbilitySpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(GrantedAbility)); 
		}
	}
}

bool UInventoryItem::TryActivateGrantedAbility(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!GrantedAbilitySpecHandle.IsValid()) return false;
	if (AbilitySystemComponent && AbilitySystemComponent->TryActivateAbility(GrantedAbilitySpecHandle)) return true;
	return false;
}

void UInventoryItem::ApplyConsumeEffect(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!ShopItem || !AbilitySystemComponent) return;
	
	TSubclassOf<UGameplayEffect> ConsumeEffect = ShopItem->GetConsumeEffect();
	if (!ConsumeEffect) return;
	
	AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(ConsumeEffect, 1, AbilitySystemComponent->MakeEffectContext());
}

void UInventoryItem::RemoveGASModifications(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!AbilitySystemComponent) return;
	if (AppliedEquippedEffectHandle.IsValid())
		AbilitySystemComponent->RemoveActiveGameplayEffect(AppliedEquippedEffectHandle); //移除效果器
	if (GrantedAbilitySpecHandle.IsValid())
		AbilitySystemComponent->SetRemoveAbilityOnEnd(GrantedAbilitySpecHandle); //等待技能结束时移除技能
}

bool UInventoryItem::IsValid() const
{
	return ShopItem != nullptr;
}

void UInventoryItem::SetSlot(int NewSlot)
{
	SlotNumber = NewSlot;
}

bool UInventoryItem::IsStackFull() const
{
	return StackCount >= GetShopItem()->GetMaxStackCount();
}

bool UInventoryItem::IsForItem(const UPA_ShopItem* Item) const
{
	if (!Item) return false;
	
	return GetShopItem() == Item;
}

bool UInventoryItem::AddStackCount()
{
	if (IsStackFull()) return false;
	++StackCount;
	return true;
}

bool UInventoryItem::ReduceStackCount()
{
	--StackCount;
	if (StackCount <= 0) return false;
	return true;
}

bool UInventoryItem::SetStackCount(int NewStackCount)
{
	if (NewStackCount > 0 && NewStackCount <= GetShopItem()->GetMaxStackCount())
	{
		StackCount = NewStackCount;
		return true;
	}
	return false;
}

bool UInventoryItem::IsGrantingAbility(TSubclassOf<class UGameplayAbility> AbilityClass) const
{
	if (!ShopItem) return false;
	TSubclassOf<UGameplayAbility> GrantedAbility = ShopItem->GetGrantedAbility();
	return GrantedAbility == AbilityClass;
}

bool UInventoryItem::IsGrantedAnyAbility() const
{
	if (!ShopItem) return false;
	return ShopItem->GetGrantedAbility() != nullptr;
}
