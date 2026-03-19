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

void UItemTreeWidget::DrawStream(bool bUpperStream, const ITreeNodeInterface* StartingNodeInterface, UUserWidget* StartingNodeWidget, 
	class UCanvasPanelSlot* StartingNodeSlot, int StartingNodeDepth, float& NextLeafXPosition, TArray<UCanvasPanelSlot*>& OutStreamSlots)
{
	TArray<const ITreeNodeInterface*> NextTreeNodeInterfaces = bUpperStream ? StartingNodeInterface->GetInputs() : StartingNodeInterface->GetOutputs();
	float StartingNodeYPos = (NodeSize.Y + NodeGap.Y) * StartingNodeDepth * (bUpperStream ? -1.0f : 1.0f); //计算节点的 Y-position
	if (NextTreeNodeInterfaces.Num() == 0) //如果是叶子节点
	{
		StartingNodeSlot->SetPosition(FVector2D{NextLeafXPosition, StartingNodeYPos}); //设置节点position
		NextLeafXPosition += NodeSize.X + NodeGap.X; //更新下一个叶子节点的 X-position
		return; //叶子节点没有子节点，直接返回
	}
	//不是叶子节点
	float NextNodeXPositionSum = 0; //记录所有节点的 X-position总和
	for (const ITreeNodeInterface* NextTreeNodeInterface : NextTreeNodeInterfaces) //遍历集合
	{
		UCanvasPanelSlot* NextWidgetSlot;
		UUserWidget* NextWidget = CreateWidgetForNode(NextTreeNodeInterface, NextWidgetSlot); //创建下一节点控件
		OutStreamSlots.Add(NextWidgetSlot); //保存下一节点的槽位信息
		if (bUpperStream)
		{
			CreateConnection(NextWidget, StartingNodeWidget); //创建下一节点与当前节点的连线
		}
		else
		{
			CreateConnection(StartingNodeWidget, NextWidget); //创建当前节点与下一节点的连线
		}
		//递归
		DrawStream(bUpperStream, NextTreeNodeInterface, NextWidget, NextWidgetSlot, StartingNodeDepth + 1, NextLeafXPosition, OutStreamSlots);
		
		NextNodeXPositionSum += NextWidgetSlot->GetPosition().X; //更新NextNodeXPositionSum
	}
	float StartingNodeXPos = NextNodeXPositionSum / NextTreeNodeInterfaces.Num(); //计算节点X-position
	StartingNodeSlot->SetPosition(FVector2D(StartingNodeXPos, StartingNodeYPos)); //设置节点位置
}