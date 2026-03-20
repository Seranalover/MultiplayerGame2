// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Widgets/ItemWidget.h"
#include "InventoryItemWidget.generated.h"

class UInventoryItem;
class UInventoryItemWidget;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemDropped, UInventoryItemWidget* /*DestinationWidget*/, UInventoryItemWidget* /*SourceWidget*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnButtonClick, const FInventoryItemHandle& /*ItemHandle*/)

/**
 * 库存物品控件原子类
 * 原子类，表示每一个单独的库存物品
 */
UCLASS()
class UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()
	
public:
	FOnInventoryItemDropped OnInventoryItemDropped; //拖拽到目标位置委托事件
	FOnButtonClick OnLeftButtonClick; //左键点击委托事件
	FOnButtonClick OnRightButtonClick; //右键点击委托事件
	
	virtual void NativeConstruct() override;
	bool IsEmpty() const;
	void UpdateInventoryItem(const UInventoryItem* Item); //更新槽位
	void EmptySlot(); //清空槽位
	FORCEINLINE int GetSlotNumber() const { return SlotNumber; }
	void SetSlotNumber(int NewSlotNumber); //设置插槽编号
	void UpdateStackCount(); //更新堆叠数
	UTexture2D* GetIconTexture() const; //查询Icon
	FORCEINLINE const UInventoryItem* GetInventoryItem() const { return InventoryItem; }
	FInventoryItemHandle GetInventoryItemHandle() const;
	
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
	
	virtual void RightButtonClicked() override; //重写右键点击事件
	virtual void LeftButtonClicked() override; //重写左键点击事件
	
	/******************************************************/
	/*                     Drag Drop                      */
	/******************************************************/
private:
	UPROPERTY(EditDefaultsOnly, Category="Drag Drop")
	TSubclassOf<class UInventoryItemDragDropOp> DragDropOpClass;
	
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override; //重写拖拽事件
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	/******************************************************/
	/*                        GAS                         */
	/******************************************************/
public:
	void StartCooldown(float Duration, float TimeRemaining); //进入冷却
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Cooldown")
	float CooldownUpdateInterval = 0.1f; //冷却更新间隔
	UPROPERTY(EditDefaultsOnly, Category="Cooldown")
	FName CooldownAmtDynamicMaterialParamName = "Percent"; //动态材质参数名-百分比
	UPROPERTY(EditDefaultsOnly, Category="Cooldown")
	FName IconTextureDynamicMaterialParamName = "Icon"; //动态材质参数名-图片
	UPROPERTY(EditDefaultsOnly, Category="Cooldown")
	FName CanCastDynamicMaterialParamName = "CanCast"; //动态材质参数名-可以施放
	
	FTimerHandle CooldownDurationTimerHandle; //总冷却时长计时器
	FTimerHandle CooldownUpdateTimerHandle;
	FNumberFormattingOptions CooldownFormattingOptions; //冷却时间格式器
	
	float CooldownRemaining = 0.0f;
	float CooldownDuration = 0.0f;
	
	void CooldownFinished();
	void UpdateCooldown();
	void ClearCooldown();
	virtual void SetIcon(UTexture2D* IconTexture) override; //重写方法，设置动态材质
};
