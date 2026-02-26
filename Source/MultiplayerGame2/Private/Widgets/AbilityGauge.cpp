// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CHeroAttributeSet.h"

void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &UAbilityGauge::AbilityCommitted);
		OwnerASC->AbilitySpecDirtiedCallbacks.AddUObject(this, &UAbilityGauge::AbilitySpecUpdated); //脏回调
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetUpgradePointAttribute()).AddUObject(this, &UAbilityGauge::UpgradePointUpdated);
	}
	bool bFound = false;
	float UpgradePoint = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetUpgradePointAttribute(), bFound);
	if (bFound)
	{
		FOnAttributeChangeData ChangeData;
		ChangeData.NewValue = UpgradePoint;
		UpgradePointUpdated(ChangeData);
	}
	OwnerAbilitySystemComponent = OwnerASC;
	WholeNumberFormattingOptions.MaximumFractionalDigits = 0;
	DigitNumberFormattingOptions.MaximumFractionalDigits = 2;
}

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);
	float CooldownDuration = UCAbilitySystemStatics::GetStaticCooldownDurationForAbility(AbilityCDO); //获得技能冷却
	float Cost = UCAbilitySystemStatics::GetStaticCostForAbility(AbilityCDO); //获得技能消耗
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration)); //设置技能冷却值文本
	CostText->SetText(FText::AsNumber(Cost)); //设置技能消耗值文本
	LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, 0); //设置初始技能等级
}

void UAbilityGauge::ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData)
{
	if (Icon && WidgetData)
	{
		Icon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, WidgetData->Icon.LoadSynchronous());
	}
}

void UAbilityGauge::AbilityCommitted(UGameplayAbility* Ability)
{
	if (Ability->GetClass()->GetDefaultObject() == AbilityCDO)
	{
		float CooldownTimeRemaining = 0.f;
		float CooldownDuration = 0.f;
		Ability->GetCooldownTimeRemainingAndDuration(Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo(),
			CooldownTimeRemaining, CooldownDuration);
		StartCooldown(CooldownTimeRemaining, CooldownDuration);
	}
}

void UAbilityGauge::StartCooldown(float CooldownTimeRemaining, float CooldownDuration)
{
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration)); //设置技能冷却值文本
	CachedCooldownDuration = CooldownDuration;
	CachedCooldownTimeRemaining = CooldownTimeRemaining;
	
	CooldownCounterText->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &UAbilityGauge::CooldownFinished, CooldownTimeRemaining);
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerUpdateHandle, this, &UAbilityGauge::UpdateCooldown, CooldownUpdateInterval, true, 0.f);
}

void UAbilityGauge::CooldownFinished()
{
	CachedCooldownDuration = CachedCooldownTimeRemaining = 0.f;
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerUpdateHandle);
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName, 1.0f ); //设置材质参数，实现冷却倒计时视觉效果

}

void UAbilityGauge::UpdateCooldown()
{
	CachedCooldownTimeRemaining -= CooldownUpdateInterval;
	FNumberFormattingOptions* FormattingOptions = CachedCooldownTimeRemaining > 1 ? &WholeNumberFormattingOptions : &DigitNumberFormattingOptions; 
	CooldownCounterText->SetText(FText::AsNumber(CachedCooldownTimeRemaining, FormattingOptions));
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName, 1.0f - CachedCooldownTimeRemaining / CachedCooldownDuration); //设置材质参数，实现冷却倒计时视觉效果
}

const FGameplayAbilitySpec* UAbilityGauge::GetAbilitySpec()
{
	if (!CachedAbilitySpec)
	{
		if (AbilityCDO && OwnerAbilitySystemComponent)
		{
			CachedAbilitySpec = OwnerAbilitySystemComponent->FindAbilitySpecFromClass(AbilityCDO->GetClass());
		}
	}
	return CachedAbilitySpec;
}

void UAbilityGauge::AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability != AbilityCDO) return;
	
	bIsAbilityLearned = AbilitySpec.Level > 0;
	LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, AbilitySpec.Level); //设置材质参数
	UpdateCanCast();
}

void UAbilityGauge::UpdateCanCast()
{
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CanCastAbilityParamName, bIsAbilityLearned ? 1.0f : 0.0f);
}

void UAbilityGauge::UpgradePointUpdated(const FOnAttributeChangeData& ChangeData)
{
	bool HasUpgradePoin = ChangeData.NewValue > 0;
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
	if (AbilitySpec)
	{
		if (UCAbilitySystemStatics::IsAbilityAtMaxLevel(*AbilitySpec))
		{
			Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvailableParamName, 0);
			return;
		}
	}
	Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvailableParamName, HasUpgradePoin ? 1.0f : 0.0f);
}
