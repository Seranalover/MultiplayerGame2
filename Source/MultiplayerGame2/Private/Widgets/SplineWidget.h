// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SplineWidget.generated.h"

/**
 * 连线控件
 * 用于在页面上连接两个商店物品，展示合成关系
 */
UCLASS()
class USplineWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetupSpline(
		const UUserWidget* InStartWidget, //起点控件
		const UUserWidget* InEndWidget, //终点控件
		const FVector2D& InStartPortLocalCoord, //起点端口坐标参数
		const FVector2D& InEndPortLocalCoord,
		const FVector2D& InStartPortDirection, //起点端口方向参数
		const FVector2D& InEndPortDirection
	); //设置绘制连线参数
	
	void SetSplineStyle(const FLinearColor& InColor, float InThickness); //设置线条样式，颜色和粗细
	
private:
	UPROPERTY(EditAnywhere, Category="Spline")
	FVector2D TestStartPosition; //测试开始位置
	
	UPROPERTY(EditAnywhere, Category="Spline")
	FVector2D TestEndPosition = FVector2D{100.f, 100.f}; //测试终点位置
	
	UPROPERTY(EditAnywhere, Category="Spline")
	FLinearColor Color = FLinearColor::White;
	
	UPROPERTY(EditAnywhere, Category="Spline")
	float Thickness = 3.f;
	
	UPROPERTY()
	const UUserWidget* StartWidget;
	
	UPROPERTY()
	const UUserWidget* EndWidget;
	
	FVector2D StartPortLocalCoord;
	FVector2D EndPortLocalCoord;
	
	UPROPERTY(EditAnywhere, Category="Spline")
	FVector2D StartPortDirection;
	
	UPROPERTY(EditAnywhere, Category="Spline")
	FVector2D EndPortDirection;
	
	//重写父类方法，绘制连线
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
};
