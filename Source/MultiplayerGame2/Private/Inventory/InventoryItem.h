// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItem.generated.h"

class UPA_ShopItem;
class UAbilitySystemComponent;

/**
 * 句柄结构体
 */
USTRUCT()
struct FInventoryItemHandle
{
	GENERATED_BODY()
public:
	FInventoryItemHandle(); //禁用公共构造函数
	static FInventoryItemHandle InvalidHandle(); //无效实例，验证用
	static FInventoryItemHandle CreateHandle(); //工厂函数，调用私有构造函数，生成唯一实例
	bool IsValid() const; //是否有效实例
	uint32 GetHandleId() const { return HandleId; } //获得句柄id
private:
	UPROPERTY()
	uint32 HandleId;
	
	explicit FInventoryItemHandle(uint32 HandleId); //仅使用私有构造函数
	static uint32 GenerateNextId(); //生成唯一id
	static uint32 GetInvalidId(); //无效id标识
};

bool operator == (const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs);
uint32 GetTypeHash(const FInventoryItemHandle& Key); //物品类型哈希映射

/**
 * 商店物品实例类
 * 仅保留对物品定义的指针
 * 
 * 存储物品原子数据、账户数据信息、授予的能力、和对应能力的句柄、效果器、和对应的效果器句柄，以及其他的必要数据信息
 * 意味着该类会非常庞大，如果通过网络直接传输该类的副本，效率很低
 * 因此通过传输inventory item handle句柄，作为唯一标识符来识别特定物品，提高效率，双方会通过该标识符创建各自的副本数据
 */
UCLASS()
class UInventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	UInventoryItem();
	const UPA_ShopItem* GetShopItem() const { return ShopItem; }
	FInventoryItemHandle GetHandle() const { return Handle; }
	
	void InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem);
	void ApplyGASModifications(UAbilitySystemComponent* AbilitySystemComponent); //GAS应用变更
	bool IsValid() const;
	FORCEINLINE int GetStackCount() const { return StackCount; } //获得堆叠数
	void SetSlot(int NewSlot); //设置槽位编号
	bool IsStackFull() const; //装备已满？
	bool IsForItem(const UPA_ShopItem* Item) const;
	bool AddStackCount(); //堆叠数+1
	bool ReduceStackCount(); //堆叠数-1
	bool SetStackCount(int NewStackCount); //设置堆叠数
	
private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;
	int StackCount; //堆叠数量
	int SlotNumber; //槽位编号
	
	FInventoryItemHandle Handle;
	FActiveGameplayEffectHandle AppliedEquippedEffectHandle;
	FGameplayAbilitySpecHandle GrantedAbilitySpecHandle;
};
