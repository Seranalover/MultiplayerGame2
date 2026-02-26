// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "LevelGauge.generated.h"

/**
 * 等级UI控件
 */
UCLASS()
class ULevelGauge : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName PercentMaterialParamName = "Percent";
	
	UPROPERTY(meta=(BindWidget))
	class UImage* LevelProgressImage; //等级进度图标
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* LevelText; //等级文本
	
	FNumberFormattingOptions LevelTextFormat; //数字格式器
	
	const UAbilitySystemComponent* OwnerASC; //能力系统组件引用
	
	void UpdateGauge(const FOnAttributeChangeData& ChangeData);
};
