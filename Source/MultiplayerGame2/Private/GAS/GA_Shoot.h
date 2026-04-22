// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_Shoot.generated.h"

/**
 * 技能 - 射击
 * 按住右键瞄准，并点击左键射击，松开右键取消整个技能
 */
UCLASS()
class UGA_Shoot : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Shoot();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	//释放输入
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Shoot")
	TSubclassOf<UGameplayEffect> ProjectileHitEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Shoot")
	float ShootProjectileSpeed = 2000.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Shoot")
	float ShootProjectileRange = 3000.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Shoot")
	TSubclassOf<class AProjectileActor> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	UAnimMontage* ShootMontage;
	
	static FGameplayTag GetShootTag(); //内部tag，发射投射物
	
	UFUNCTION()
	void StartShooting(FGameplayEventData Payload);
	UFUNCTION()
	void StopShooting(FGameplayEventData Payload);
	UFUNCTION()
	void ShootProjectile(FGameplayEventData Payload); //发射投射物
};
