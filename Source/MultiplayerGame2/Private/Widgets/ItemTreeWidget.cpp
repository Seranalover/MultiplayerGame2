// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemTreeWidget.h"

#include "SplineWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UItemTreeWidget::ClearTree()
{
	RootPanel->ClearChildren();
}

UUserWidget* UItemTreeWidget::CreateWidgetForNode(const ITreeNodeInterface* Node, class UCanvasPanelSlot*& OutCanvasSlot)
{
	if (!Node) return nullptr;
	
	UUserWidget* NodeWidget = Node->GetWidget();
	OutCanvasSlot = RootPanel->AddChildToCanvas(NodeWidget); //添加并获得节点的槽位，里面包含节点的位置信息
	if (OutCanvasSlot)
	{
		OutCanvasSlot->SetSize(NodeSize);
		OutCanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f)); //设置节点槽位锚定到画布中心点
		OutCanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f)); //设置对齐
		OutCanvasSlot->SetZOrder(1); //设置节点显示层级为 1
	}
	return NodeWidget;
}

void UItemTreeWidget::CreateConnection(const UUserWidget* From, const UUserWidget* To)
{
	if (!From || !To) return;
	
	USplineWidget* Connection = CreateWidget<USplineWidget>(GetOwningPlayer());
	UCanvasPanelSlot* ConnectionPanelSlot = RootPanel->AddChildToCanvas(Connection);
	
	if (ConnectionPanelSlot)
	{
		ConnectionPanelSlot->SetAnchors(FAnchors(0.f, 0.f));
		ConnectionPanelSlot->SetAlignment(FVector2D(0.f, 0.f));
		ConnectionPanelSlot->SetPosition(FVector2D::Zero());
		ConnectionPanelSlot->SetZOrder(0); //设置连线显示层级为 0
	}
	Connection->SetupSpline(From, To, SourcePortLocalPos, 
		DestinationPortLocalPos, SourcePortDirection, DestinationPortLocalPos); //绘制连线
	Connection->SetSplineStyle(ConnectionColor, ConnectionThickness); //设置连线样式
}
