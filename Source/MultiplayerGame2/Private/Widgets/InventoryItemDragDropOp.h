// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryItemDragDropOp.generated.h"

class UItemWidget;
class UInventoryItemWidget;
/**
 * 装备栏物品拖放操作类
 * 代表当前正在拖拽的对象，携带对象信息，显示视觉效果
 */
UCLASS()
class UInventoryItemDragDropOp : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	void SetDraggedItem(UInventoryItemWidget* DraggedItem); //设置拖拽对象
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	TSubclassOf<UItemWidget> DragVisualClass;
};
