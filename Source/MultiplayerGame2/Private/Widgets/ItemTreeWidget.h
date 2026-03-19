// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TreeNodeInterface.h"
#include "Blueprint/UserWidget.h"
#include "ItemTreeWidget.generated.h"

/**
 * 商品树结构图控件
 */
UCLASS()
class UItemTreeWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* RootPanel; //根面板
	
	UPROPERTY(EditDefaultsOnly, Category="Tree")
	FVector2D NodeSize = FVector2D{60.f}; //节点尺寸大小
	
	UPROPERTY(EditDefaultsOnly, Category="Tree")
	FVector2D NodeGap = FVector2D{16.f, 30.f}; //节点间距
	
	UPROPERTY(EditDefaultsOnly, Category="Tree")
	FLinearColor ConnectionColor = FLinearColor{0.8f, 0.8f, 0.8f, 1.f}; //连线
	
	UPROPERTY(EditDefaultsOnly, Category="Tree")
	float ConnectionThickness = 3.f; //连线粗细
	
	UPROPERTY(EditDefaultsOnly, Category="Tree")
	FVector2D SourcePortLocalPos = FVector2D{0.5f, 0.9f}; //父节点位置
	
	UPROPERTY(EditDefaultsOnly, Category="Tree")
	FVector2D DestinationPortLocalPos = FVector2D{0.5f, 0.1f}; //子节点位置
	
	UPROPERTY(EditDefaultsOnly, Category="Tree")
	FVector2D SourcePortDirection = FVector2D{0.f, 90.f}; //父节点连线方向
	
	UPROPERTY(EditDefaultsOnly, Category="Tree")
	FVector2D DestinationPortDirection = FVector2D{0.f, 90.f}; //子节点连线方向
	
	void ClearTree(); //清空树
	UUserWidget* CreateWidgetForNode(const ITreeNodeInterface* Node, class UCanvasPanelSlot*& OutCanvasSlot); //为节点创建控件
	void CreateConnection(const UUserWidget* From, const UUserWidget* To); //创建连线
};
