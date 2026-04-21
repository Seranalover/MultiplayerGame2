// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "GameplayWidget.generated.h"

/**
 * HUD
 */
UCLASS()
class UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override; 
	void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities);
	void ToggleShop(); //开关商店
	
private:
	UPROPERTY(meta=(BindWidget))
	class UValueGauge* HealthBar;
	
	UPROPERTY(meta=(BindWidget))
	class UValueGauge* ManaBar;
	
	UPROPERTY(meta=(BindWidget))
	class UAbilitiesListView* AbilitiesListView; //技能栏列表
	
	UPROPERTY(meta=(BindWidget))
	class UStatsGauge* AttackDamageGauge;
	
	UPROPERTY(meta=(BindWidget))
	class UStatsGauge* ArmorGauge;
	
	UPROPERTY(meta=(BindWidget))
	class UStatsGauge* MoveSpeedGauge;
	
	UPROPERTY(meta=(BindWidget))
	class UStatsGauge* StrengthGauge;
	
	UPROPERTY(meta=(BindWidget))
	class UStatsGauge* IntelligenceGauge;
	
	UPROPERTY(meta=(BindWidget))
	class UShopWidget* ShopWidget; //商店页面
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	class UWidgetAnimation* ShopPopupAnimation; //商店页面淡入淡出动画
	
	UPROPERTY(meta=(BindWidget))
	class UInventoryWidget* InventoryWidget; //装备栏控件
	
	UPROPERTY(meta=(BindWidget))
	class USkeletalMeshRenderWidget* HeadshotWidget;
	
	UPROPERTY(meta=(BindWidget))
	class UMatchStatWidget* MatchStatWidget;
	
	UPROPERTY()
	class UAbilitySystemComponent* OwnerAbilitySystemComponent;
	
	void PlayShopPopupAnimation(bool bPlayForward);
	void SetOwningPawnInputEnabled(bool bEnable); //用于打开商店时，禁用Pawn操作输入
	void SetShowMouseCursor(bool bShow); //用于打开商店时，显示鼠标
	void SetFocusToGameAndUI(); //打开商店时，设置焦点为游戏和UI
	void SetFocusToGameOnly(); //关闭商店时，设置焦点为仅游戏
};
