// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "GAS/CAttributeSet.h"
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
	//设置和绑定GAS控件的属性值
	void SetAndBoundToGameplayAttribute(class UAbilitySystemComponent* AbilitySystemComponent, 
		const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute); 
	void SetValue(float NewValue, float NewMaxValue);
	
private:
	void ValueChanged(const FOnAttributeChangeData& Data); //订阅事件中应用更改
	void MaxValueChanged(const FOnAttributeChangeData& Data); //订阅事件中应用更改
	
	float CachedValue; //缓存变更值
	float CachedMaxValue; //缓存变更最大值
	
	UPROPERTY(EditAnywhere, Category="Visual")
	FLinearColor BarColor; //进度条颜色
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UProgressBar* ProgressBar; //进度条
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* ValueText; //文本
	
	
};
