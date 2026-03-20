// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemToolTip.h"
#include "Blueprint/UserWidget.h"
#include "ItemWidget.generated.h"

/**
 * 物品控件类
 * 处理物品公用功能，如设置图标，处理右键点击事件等
 */
UCLASS()
class UItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void SetIcon(UTexture2D* IconTexture); //这里设置为virtual，方便以后子类重写方法，实现使用装备主动技能时，图标显示倒计时等功能
	
protected:
	UItemToolTip* SetToolTipWidget(const UPA_ShopItem* Item);
	class UImage* GetItemIcon() const { return ItemIcon; }
	
private:
	UPROPERTY(meta=(BindWidget))
	class UImage* ItemIcon;
	
	UPROPERTY(EditDefaultsOnly, Category="ToolTip")
	TSubclassOf<UItemToolTip> ItemToolTipClass;
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override; //鼠标按下
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override; //鼠标释放
	virtual void RightButtonClicked(); //点击右键
	virtual void LeftButtonClicked(); //点击左键
};
