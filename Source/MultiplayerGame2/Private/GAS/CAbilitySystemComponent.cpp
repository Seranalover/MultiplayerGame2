// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CAbilitySystemStatics.h"
#include "CAttributeSet.h"
#include "CHeroAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "PA_AbilitySystemGenerics.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &UCAbilitySystemComponent::HealthUpdated); //监听attribute change，绑定HealthUpdated()函数
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &UCAbilitySystemComponent::ManaUpdated); //监听attribute change，绑定ManaUpdated()函数
	GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &UCAbilitySystemComponent::ExperienceUpdated); //监听attribute change，绑定ExperienceUpdated()函数
	GenericConfirmInputID = (int32)ECAbilityInputID::Confirm; //技能确认输入
	GenericCancelInputID = (int32)ECAbilityInputID::Cancel; //技能取消输入
}

void UCAbilitySystemComponent::ServerSideInit()
{
	InitializeBaseAttributes();
	ApplyInitialEffects();
	GiveInitialAbilities();
}

void UCAbilitySystemComponent::InitializeBaseAttributes()
{
	if (!AbilitySystemGenerics || !AbilitySystemGenerics->GetBaseStatsDataTable() || !GetOwner()) return;
	const UDataTable* BaseStatsDataTable = AbilitySystemGenerics->GetBaseStatsDataTable();
	const FHeroBaseStats* BaseStats = nullptr;
	for (const TPair<FName, uint8*>& DataPair : BaseStatsDataTable->GetRowMap())
	{
		BaseStats = BaseStatsDataTable->FindRow<FHeroBaseStats>(DataPair.Key, "");
		if (BaseStats && BaseStats->Class == GetOwner()->GetClass())
			break;
	}
	if (BaseStats)
	{
		SetNumericAttributeBase(UCAttributeSet::GetMaxHealthAttribute(), BaseStats->BaseMaxHealth);
		SetNumericAttributeBase(UCAttributeSet::GetMaxManaAttribute(), BaseStats->BaseMaxMana);
		SetNumericAttributeBase(UCAttributeSet::GetAttackDamageAttribute(), BaseStats->BaseAttackDamage);
		SetNumericAttributeBase(UCAttributeSet::GetArmorAttribute(), BaseStats->BaseArmor);
		SetNumericAttributeBase(UCAttributeSet::GetMoveSpeedAttribute(), BaseStats->BaseMoveSpeed);
		
		SetNumericAttributeBase(UCHeroAttributeSet::GetStrengthAttribute(), BaseStats->Strength);
		SetNumericAttributeBase(UCHeroAttributeSet::GetStrengthGrowthRateAttribute(), BaseStats->StrengthGrowthRate);
		SetNumericAttributeBase(UCHeroAttributeSet::GetIntelligenceAttribute(), BaseStats->Intelligence);
		SetNumericAttributeBase(UCHeroAttributeSet::GetIntelligenceGrowthRateAttribute(), BaseStats->IntelligenceGrowthRate);
	}
	const FRealCurve* ExperienceCurve = AbilitySystemGenerics->GetExperienceCurve();
	if (ExperienceCurve) //根据ExperienceCurve数据表初始化最高等级和经验
	{
		int MaxLevel = ExperienceCurve->GetNumKeys();
		SetNumericAttributeBase(UCHeroAttributeSet::GetMaxLevelAttribute(), MaxLevel);
		float MaxExp = ExperienceCurve->GetKeyValue(ExperienceCurve->GetLastKeyHandle());
		SetNumericAttributeBase(UCHeroAttributeSet::GetMaxLevelExperienceAttribute(), MaxExp);
	}
	ExperienceUpdated(FOnAttributeChangeData());
}

void UCAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return; //server only
	if (!AbilitySystemGenerics) return;
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : AbilitySystemGenerics->GetInitialEffects())
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext()); 
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UCAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return; //server only
	
	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, (int32)AbilityPair.Key, nullptr)); //赋予附加能力
	}
	
	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& BasicAbilityPair : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(BasicAbilityPair.Value, 1, (int32)BasicAbilityPair.Key, nullptr)); //赋予基础能力
	}
	
	if (!AbilitySystemGenerics) return;
	
	for (const TSubclassOf<UGameplayAbility>& PassiveAbility : AbilitySystemGenerics->GetPassiveAbilities())
	{
		GiveAbility(FGameplayAbilitySpec(PassiveAbility, 1, -1, nullptr)); //赋予被动技能
	}
}

void UCAbilitySystemComponent::ApplyFullStatEffect()
{
	if (!AbilitySystemGenerics) return;
	AuthApplyGameplayEffect(AbilitySystemGenerics->GetFullStatEffect());
}

const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& UCAbilitySystemComponent::GetAbilities() const
{
	return Abilities;
}

bool UCAbilitySystemComponent::IsAtMaxLevel() const
{
	bool bFound;
	float CurrentLevel = GetGameplayAttributeValue(UCHeroAttributeSet::GetLevelAttribute(), bFound);
	float MaxLevel = GetGameplayAttributeValue(UCHeroAttributeSet::GetMaxLevelAttribute(), bFound);
	return CurrentLevel >= MaxLevel;
}

void UCAbilitySystemComponent::Server_UpgradeAbilityWithInputID_Implementation(ECAbilityInputID InputID)
{
	bool bFound;
	float UpgradePoint = GetGameplayAttributeValue(UCHeroAttributeSet::GetUpgradePointAttribute(), bFound);
	if (!bFound || UpgradePoint <= 0) return;
	
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromInputID((int32)InputID);
	if (!AbilitySpec || UCAbilitySystemStatics::IsAbilityAtMaxLevel(*AbilitySpec)) return;
	
	SetNumericAttributeBase(UCHeroAttributeSet::GetUpgradePointAttribute(), UpgradePoint - 1); //技能点 -1
	AbilitySpec->Level += 1; //技能等级 +1
	MarkAbilitySpecDirty(*AbilitySpec); //触发spec dirty回调
	Client_AbilitySpecLevelUpdated(AbilitySpec->Handle, AbilitySpec->Level);
}

bool UCAbilitySystemComponent::Server_UpgradeAbilityWithInputID_Validate(ECAbilityInputID InputID)
{
	return true;
}

void UCAbilitySystemComponent::Client_AbilitySpecLevelUpdated_Implementation(FGameplayAbilitySpecHandle Handle, int NewLevel)
{
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
	if (AbilitySpec)
	{
		AbilitySpec->Level = NewLevel; //client技能等级 +1
		AbilitySpecDirtiedCallbacks.Broadcast(*AbilitySpec); //spec dirty回调
	}
}

void UCAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	bool bFound = false;
	float MaxHealth = GetGameplayAttributeValue(UCAttributeSet::GetMaxHealthAttribute(), bFound);
	if (bFound && ChangeData.NewValue >= MaxHealth)
	{
		if (!HasMatchingGameplayTag(UCAbilitySystemStatics::GetHealthFullStatTag()))
			AddLooseGameplayTag(UCAbilitySystemStatics::GetHealthFullStatTag()); //local only
	}
	else
	{
		if (HasMatchingGameplayTag(UCAbilitySystemStatics::GetHealthFullStatTag()))
			RemoveLooseGameplayTag(UCAbilitySystemStatics::GetHealthFullStatTag()); //local only
	}
	
	if (ChangeData.NewValue <= 0)
	{
		if (!HasMatchingGameplayTag(UCAbilitySystemStatics::GetHealthEmptyStatTag()))
		{
			AddLooseGameplayTag(UCAbilitySystemStatics::GetHealthEmptyStatTag()); //local only
			if (AbilitySystemGenerics && AbilitySystemGenerics->GetDeathEffect())
				AuthApplyGameplayEffect(AbilitySystemGenerics->GetDeathEffect());
			
			FGameplayEventData DeathAbilityEventData; //死亡事件信息，传递给GAP_Death中
			if (ChangeData.GEModData)
				DeathAbilityEventData.ContextHandle = ChangeData.GEModData->EffectSpec.GetContext(); //从传入参数中获得上下文句柄
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), UCAbilitySystemStatics::GetDeadStatTag(), DeathAbilityEventData); //传递死亡事件
		}
		else
		{
			if (HasMatchingGameplayTag(UCAbilitySystemStatics::GetHealthEmptyStatTag()))
				RemoveLooseGameplayTag(UCAbilitySystemStatics::GetHealthEmptyStatTag()); //local only
		}
	}
}

void UCAbilitySystemComponent::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	bool bFound = false;
	float MaxMana = GetGameplayAttributeValue(UCAttributeSet::GetMaxManaAttribute(), bFound);
	if (bFound && ChangeData.NewValue >= MaxMana)
	{
		if (!HasMatchingGameplayTag(UCAbilitySystemStatics::GetManaFullStatTag()))
			AddLooseGameplayTag(UCAbilitySystemStatics::GetManaFullStatTag()); //local only
	}
	else
	{
		if (HasMatchingGameplayTag(UCAbilitySystemStatics::GetManaFullStatTag()))
			RemoveLooseGameplayTag(UCAbilitySystemStatics::GetManaFullStatTag()); //local only
	}
	
	if (ChangeData.NewValue <= 0)
	{
		if (!HasMatchingGameplayTag(UCAbilitySystemStatics::GetManaEmptyStatTag()))
		{
			AddLooseGameplayTag(UCAbilitySystemStatics::GetManaEmptyStatTag()); //local only
		}
		else
		{
			if (HasMatchingGameplayTag(UCAbilitySystemStatics::GetManaEmptyStatTag()))
				RemoveLooseGameplayTag(UCAbilitySystemStatics::GetManaEmptyStatTag()); //local only
		}
	}
}

void UCAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GameplayEffect, Level, MakeEffectContext()); 
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());	
	}
}

void UCAbilitySystemComponent::ExperienceUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (IsAtMaxLevel()) return;
	if (!AbilitySystemGenerics) return;
	float CurrentExp = ChangeData.NewValue;
	const FRealCurve* ExperienceCurve = AbilitySystemGenerics->GetExperienceCurve();
	if (!ExperienceCurve) return;
	
	float PrevLevelExp = 0;
	float NextLevelExp = 0;
	float NewLevel = 1;
	
	for (auto Iter = ExperienceCurve->GetKeyHandleIterator(); Iter; ++Iter)
	{
		float ExperienceToReachLevel = ExperienceCurve->GetKeyValue(*Iter);
		if (CurrentExp < ExperienceToReachLevel)
		{
			NextLevelExp = ExperienceToReachLevel;
			break;
		}
		PrevLevelExp = ExperienceToReachLevel;
		NewLevel = Iter.GetIndex() + 1;
	}
	float CurrentLevel = GetNumericAttributeBase(UCHeroAttributeSet::GetLevelAttribute());
	float CurrentUpgradePoint = GetNumericAttribute(UCHeroAttributeSet::GetUpgradePointAttribute());
	float LevelUpgraded = NewLevel - CurrentLevel;
	float NewUpgradePoint = CurrentUpgradePoint + LevelUpgraded;
	SetNumericAttributeBase(UCHeroAttributeSet::GetLevelAttribute(), NewLevel);
	SetNumericAttributeBase(UCHeroAttributeSet::GetPrevLevelExperienceAttribute(), PrevLevelExp);
	SetNumericAttributeBase(UCHeroAttributeSet::GetNextLevelExperienceAttribute(), NextLevelExp);
	SetNumericAttributeBase(UCHeroAttributeSet::GetUpgradePointAttribute(), NewUpgradePoint);
}
