// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "GAS/CAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "AbilityGauge.generated.h"

class UAbilitySystemComponent;
struct FGameplayAbilitySpec;

/**
 * data table 结构体
 */
USTRUCT(blueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UGameplayAbility> AbilityClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AbilityName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon; //Soft指针，仅保留软引用资源到内存，需要访问时才加载到内存，节省开销
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};

/**
 * 技能图标UI基础类
 */
UCLASS()
class UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	void ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData);
	
private:
	UPROPERTY(meta=(BindWidget))
	class UImage* Icon;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownCounterText; //冷却计数
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownDurationText; //冷却持续时间
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CostText; //消耗
	
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName IconMaterialParamName = "Icon"; //材质名
	
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName CooldownPercentParamName = "Percent"; 
	
	UPROPERTY()
	class UGameplayAbility* AbilityCDO; //Ability类默认对象
	
	UPROPERTY(EditDefaultsOnly, Category="Cooldown")
	float CooldownUpdateInterval = 0.1f;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* LevelGauge; //技能等级
	
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName AbilityLevelParamName = "Level";
	
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName CanCastAbilityParamName = "CanCast";
	
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName UpgradePointAvailableParamName = "UpgradeAvailable";
	
	float CachedCooldownDuration;
	float CachedCooldownTimeRemaining;
	FTimerHandle CooldownTimerHandle;
	FTimerHandle CooldownTimerUpdateHandle;
	FNumberFormattingOptions WholeNumberFormattingOptions; //冷却剩余时间 整数格式器
	FNumberFormattingOptions DigitNumberFormattingOptions; //冷却剩余时间 小数格式器
	
	void AbilityCommitted(UGameplayAbility* Ability);
	void StartCooldown(float CooldownTimeRemaining,float CooldownDuration);
	
	void CooldownFinished();
	void UpdateCooldown();
	
	const UAbilitySystemComponent* OwnerAbilitySystemComponent; //ASC组件
	FGameplayAbilitySpecHandle CachedAbilitySpecHandle; //Ability组件
	
	const FGameplayAbilitySpec* GetAbilitySpec(); //CachedAbilitySpec不存在时加载AbilitySpec
	
	bool bIsAbilityLearned = false;
	
	void AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec);
	void UpdateCanCast();
	void UpgradePointUpdated(const FOnAttributeChangeData& ChangeData);
	void ManaUpdated(const FOnAttributeChangeData& ChangeData);
	
	UPROPERTY(EditDefaultsOnly, Category="Tool Tip")
	TSubclassOf<class UAbilityToolTip> AbilityToolTipClass;
	
	void createToolTipWidget(const FAbilityWidgetData* AbilityWidgetData);
};
