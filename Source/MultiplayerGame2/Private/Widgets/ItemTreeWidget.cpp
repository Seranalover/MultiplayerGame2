// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemTreeWidget.h"

#include "SplineWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UItemTreeWidget::DrawFromNode(const ITreeNodeInterface* NodeInterface)
{
	if (!NodeInterface) return;
	if (CurrentCenterItem == NodeInterface->GetItemObject()) return;
	
	ClearTree();
	CurrentCenterItem = NodeInterface->GetItemObject(); //记录已绘制的中心节点，避免重复绘制
	
	//设置参数
	float NextLeafXPosition = 0.f;
	UCanvasPanelSlot* CenterWidgetPanelSlot = nullptr;
	UUserWidget* CenterWidget = CreateWidgetForNode(NodeInterface, CenterWidgetPanelSlot);
	TArray<UCanvasPanelSlot*> LowerStreamSlots, UpperStreamSlots;
	
	//向下绘制树状图
	DrawStream(false, NodeInterface, CenterWidget, CenterWidgetPanelSlot, 0, NextLeafXPosition, LowerStreamSlots);
	float LowerStreamXMax = NextLeafXPosition - NodeSize.X - NodeGap.X; //计算向下流向的，最右边叶子节点位置
	float LowerMoveAmt = 0.f - LowerStreamXMax / 2.f; //计算下流向的，使整个树居中，需要向右移动的距离
	for (UCanvasPanelSlot* StreamSlot : LowerStreamSlots) //遍历移动所有子节点，使树状图居中
	{
		StreamSlot->SetPosition(StreamSlot->GetPosition() + FVector2D{LowerMoveAmt, 0.f});
	}
	
	//向上绘制树状图
	NextLeafXPosition = 0.f;
	DrawStream(true, NodeInterface, CenterWidget, CenterWidgetPanelSlot, 0, NextLeafXPosition, LowerStreamSlots);
	float  UpperStreamXMax = NextLeafXPosition - NodeSize.X - NodeGap.X;
	float  UpperMoveAmt = 0.f -  UpperStreamXMax / 2.f;
	for (UCanvasPanelSlot* StreamSlot :  UpperStreamSlots)
	{
		StreamSlot->SetPosition(StreamSlot->GetPosition() + FVector2D{ UpperMoveAmt, 0.f});
	}
	
	CenterWidgetPanelSlot->SetPosition(FVector2D::Zero());
}

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
		DestinationPortLocalPos, SourcePortDirection, DestinationPortDirection); //绘制连线
	Connection->SetSplineStyle(ConnectionColor, ConnectionThickness); //设置连线样式
}

void UItemTreeWidget::DrawStream(bool bUpperStream, const ITreeNodeInterface* NodeInterface, UUserWidget* NodeWidget, 
	class UCanvasPanelSlot* NodeSlot, int NodeDepth, float& NextLeafXPosition, TArray<UCanvasPanelSlot*>& OutStreamSlots)
{
	TArray<const ITreeNodeInterface*> NextTreeNodeInterfaces = bUpperStream ? NodeInterface->GetInputs() : NodeInterface->GetOutputs();
	float StartingNodeYPos = (NodeSize.Y + NodeGap.Y) * NodeDepth * (bUpperStream ? -1.0f : 1.0f); //计算节点的 Y-position
	if (NextTreeNodeInterfaces.Num() == 0) //如果是叶子节点
	{
		NodeSlot->SetPosition(FVector2D{NextLeafXPosition, StartingNodeYPos}); //设置节点position
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
			CreateConnection(NextWidget, NodeWidget); //创建下一节点与当前节点的连线
		}
		else
		{
			CreateConnection(NodeWidget, NextWidget); //创建当前节点与下一节点的连线
		}
		DrawStream(bUpperStream, NextTreeNodeInterface, NextWidget, NextWidgetSlot, NodeDepth + 1, NextLeafXPosition, OutStreamSlots); //递归
		NextNodeXPositionSum += NextWidgetSlot->GetPosition().X; //更新NextNodeXPositionSum
	}
	float StartingNodeXPos = NextNodeXPositionSum / NextTreeNodeInterfaces.Num(); //计算节点X-position
	NodeSlot->SetPosition(FVector2D(StartingNodeXPos, StartingNodeYPos)); //设置节点位置
}