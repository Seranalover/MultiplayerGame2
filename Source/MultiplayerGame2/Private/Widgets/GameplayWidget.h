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
	
	UPROPERTY()
	class UAbilitySystemComponent* OwnerAbilitySystemComponent;
	
	void PlayShopPopupAnimation(bool bPlayForward);
};
