// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SplineWidget.h"

void USplineWidget::SetupSpline(const UUserWidget* InStartWidget, const UUserWidget* InEndWidget,
	const FVector2D& InStartPortLocalCoord, const FVector2D& InEndPortLocalCoord, const FVector2D& InStartPortDirection,
	const FVector2D& InEndPortDirection)
{
	StartWidget = InStartWidget;
	EndWidget = InEndWidget;
	StartPortLocalCoord = InStartPortLocalCoord;
	EndPortLocalCoord = InEndPortLocalCoord;
	StartPortDirection = InStartPortDirection;
	EndPortDirection = InEndPortDirection;
}

void USplineWidget::SetSplineStyle(const FLinearColor& InColor, float InThickness)
{
	Color = InColor;
	Thickness = InThickness;
}

int32 USplineWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	//LayerId表示绘制的线条位于哪个图层，越高显示层级就越靠上
	LayerId = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	
	FVector2D StartPosition = TestStartPosition;
	FVector2D EndPosition = TestEndPosition;
	
	if (StartWidget && EndWidget)
	{
		StartPosition = StartWidget->GetCachedGeometry().GetLocalPositionAtCoordinates(StartPortLocalCoord);
		EndPosition = EndWidget->GetCachedGeometry().GetLocalPositionAtCoordinates(EndPortLocalCoord);
	}
	
	//绘制连线
	FSlateDrawElement::MakeSpline(OutDrawElements, ++LayerId, AllottedGeometry.ToPaintGeometry(), StartPosition,
		StartPortDirection, EndPosition, EndPortDirection, Thickness, ESlateDrawEffect::None, Color);
	
	return LayerId;
}
