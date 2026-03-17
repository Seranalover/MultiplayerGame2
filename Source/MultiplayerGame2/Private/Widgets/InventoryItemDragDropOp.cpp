// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryItemDragDropOp.h"
#include "Widgets/InventoryItemWidget.h"

void UInventoryItemDragDropOp::SetDraggedItem(UInventoryItemWidget* DraggedItem)
{
	Payload = DraggedItem; //将拖拽对象控件存入内部变量Payload
	if (DragVisualClass)
	{
		UItemWidget* DragItemWidget = CreateWidget<UItemWidget>(GetWorld(), DragVisualClass); //创建视觉元素对象
		if (DragItemWidget)
		{
			DragItemWidget->SetIcon(DraggedItem->GetIconTexture());
			DefaultDragVisual = DragItemWidget; //指定为默认拖拽视觉对象
		}
	}
}
