// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LevelGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/CHeroAttributeSet.h"

void ULevelGauge::NativeConstruct()
{
	Super::NativeConstruct();
	LevelTextFormat.SetMaximumFractionalDigits(0);
	
	APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!OwnerPawn) return;
	
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPawn); //获得非引用的ASC组件，监听变化
	if (!AbilitySystemComponent) return;
	
	OwnerASC = AbilitySystemComponent;
	
	UpdateGauge(FOnAttributeChangeData());
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UCHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge); //监听经验值变化，绑定函数
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UCHeroAttributeSet::GetNextLevelExperienceAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UCHeroAttributeSet::GetPrevLevelExperienceAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UCHeroAttributeSet::GetLevelAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
}

void ULevelGauge::UpdateGauge(const FOnAttributeChangeData& ChangeData)
{
	//计算并设置等级进度条
	bool bFound;
	float CurrentExperience = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetExperienceAttribute(), bFound);
	if (!bFound) return;
	float NextLevelExperience = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetNextLevelExperienceAttribute(), bFound);
	if (!bFound) return;
	float PrevLevelExperience = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetPrevLevelExperienceAttribute(), bFound);
	if (!bFound) return;
	float CurrentLevel = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetLevelAttribute(), bFound);
	if (!bFound) return;
	
	LevelText->SetText(FText::AsNumber(CurrentLevel, &LevelTextFormat));
	
	float Progress = CurrentExperience - PrevLevelExperience;
	float LevelExpAmt = NextLevelExperience - PrevLevelExperience;
	float Percent = Progress / LevelExpAmt;
	
	if (NextLevelExperience == 0) //是否满级
		Percent = 1;
	if (LevelProgressImage)
		LevelProgressImage->GetDynamicMaterial()->SetScalarParameterValue(PercentMaterialParamName, Percent); 
}
