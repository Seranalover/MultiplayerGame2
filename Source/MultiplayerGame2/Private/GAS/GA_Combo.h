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

private:
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* ComboMontage;
	
	UFUNCTION()
	void GetComboChangedEventReceived(FGameplayEventData Data);
	
	FName NextComboName; //下一段连招名
};
