// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Widgets/ItemWidget.h"
#include "InventoryItemWidget.generated.h"

class UInventoryItem;

/**
 * 库存物品控件原子类
 * 原子类，表示每一个单独的库存物品
 */
UCLASS()
class UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	bool IsEmpty() const;
	void UpdateInventoryItem(const UInventoryItem* Item); //更新槽位
	void EmptySlot(); //清空槽位
	FORCEINLINE int GetSlotNumber() const { return SlotNumber; }
	void SetSlotNumber(int NewSlotNumber); //设置插槽编号
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	UTexture2D* EmptyTexture; //未购买装备时的默认纹理图片
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* StackCountText; //堆叠数量
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownCountText; //冷却时间
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownDurationText; //冷却持续时间
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ManaCostText; //魔力消耗
	
	UPROPERTY()
	const UInventoryItem* InventoryItem; //物品指针
	
	int SlotNumber; //槽位编号
	
	void UpdateStackCount();
};
