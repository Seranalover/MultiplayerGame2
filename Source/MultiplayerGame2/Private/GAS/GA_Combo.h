// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_Combo.generated.h"

/**
 * 连击类
 */
UCLASS()
class UGA_Combo : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Combo();
	//重要函数ActivateAbility，激活并执行技能
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	static FGameplayTag GetComboChangedEventTag(); //获得切换连招的事件tag
	static FGameplayTag GetComboChangedEventEndTag(); //获得结束连招的事件tag
	static FGameplayTag GetComboTargetEventTag(); //combo扫描事件tag

private:
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* ComboMontage; //连招蒙太奇
	
	UFUNCTION()
	void GetComboChangedEventReceived(FGameplayEventData Data); //接收重置连招事件
	FName NextComboName; //下一段连招名
	
	void SetupWaitComboInputPress();
	void TryCommitCombo(); //尝试切换到下一段连招
	UFUNCTION()
	void HandleInputPress(float TimeWaited); //按下输入处理实现
	
	UFUNCTION()
	void DoDamage(FGameplayEventData Data); //combo攻击实现
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effect")
	TMap<FName, TSubclassOf<UGameplayEffect>> DamageEffectMap; //攻击效果映射集
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effect")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect; //默认攻击效果
	
	TSubclassOf<UGameplayEffect> GetDamageEffectForCurrentCombo() const; //当前连段的攻击效果
};
