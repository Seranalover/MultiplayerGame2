// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ValueGauge.generated.h"

/**
 * 状态值计量条控件
 */
UCLASS()
class UValueGauge : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override; //同蓝图中的PreConstruct
	void SetValue(float NewValue, float NewMaxValue);
	
private:
	UPROPERTY(EditAnywhere, Category="Visual")
	FLinearColor BarColor; //进度条颜色
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UProgressBar* ProgressBar; //进度条
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* ValueText; //文本
};
