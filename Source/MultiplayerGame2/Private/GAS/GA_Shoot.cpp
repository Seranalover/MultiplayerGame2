// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Shoot.h"

#include "CAbilitySystemStatics.h"
#include "GameplayTagsManager.h"
#include "ProjectileActor.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UGA_Shoot::UGA_Shoot()
{
	ActivationOwnedTags.AddTag(UCAbilitySystemStatics::GetAimStatTag());
	ActivationOwnedTags.AddTag(UCAbilitySystemStatics::GetCrosshairTag());
}

void UGA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	// UE_LOG(LogTemp, Warning, TEXT("shoot ability activated"));
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		//开始射击
		UAbilityTask_WaitGameplayEvent* WaitStartShootingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UCAbilitySystemStatics::GetBasicAttackInputPressedTag());
		WaitStartShootingEvent->EventReceived.AddDynamic(this, &UGA_Shoot::StartShooting);
		WaitStartShootingEvent->ReadyForActivation();
		
		//停止射击
		UAbilityTask_WaitGameplayEvent* WaitStopShootingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UCAbilitySystemStatics::GetBasicAttackInputReleasedTag());
		WaitStopShootingEvent->EventReceived.AddDynamic(this, &UGA_Shoot::StopShooting);
		WaitStopShootingEvent->ReadyForActivation();
		
		//发射投射物
		UAbilityTask_WaitGameplayEvent* WaitShootProjectileEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetShootTag(), nullptr, false, false);
		WaitShootProjectileEvent->EventReceived.AddDynamic(this, &UGA_Shoot::ShootProjectile);
		WaitShootProjectileEvent->ReadyForActivation();
	}
}

void UGA_Shoot::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("shoot ability ended"));
	K2_EndAbility();
}

FGameplayTag UGA_Shoot::GetShootTag()
{
	return FGameplayTag::RequestGameplayTag("ability.shoot");
}

void UGA_Shoot::StartShooting(FGameplayEventData Payload)
{
	// UE_LOG(LogTemp, Warning, TEXT("start shooting"));
	if (HasAuthority(&CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayShootMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ShootMontage);
		PlayShootMontage->ReadyForActivation();
	}
	else
	{
		PlayMontageLocally(ShootMontage);
	}
}

void UGA_Shoot::StopShooting(FGameplayEventData Payload)
{
	// UE_LOG(LogTemp, Warning, TEXT("stop shooting"));
	if (ShootMontage)
	{
		StopMontageAfterCurrentSection(ShootMontage);
	}
}

void UGA_Shoot::ShootProjectile(FGameplayEventData Payload)
{
	// UE_LOG(LogTemp, Warning, TEXT("shoot projectile"));
	if (K2_HasAuthority())
	{
		AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerAvatarActor;
		SpawnParams.SpawnCollisionHandlingOverride  = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		//获得socket
		FVector SocketLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
		USkeletalMeshComponent* MeshComponent = GetOwningComponentFromActorInfo();
		if (MeshComponent)
		{
			TArray<FName> OutNames;
			UGameplayTagsManager::Get().SplitGameplayTagFName(Payload.EventTag, OutNames);
			if (OutNames.Num() != 0)
			{
				FName SocketName = OutNames.Last();
				SocketLocation = MeshComponent->GetSocketLocation(SocketName);
			}
		}
		
		AProjectileActor* ProjectileActor = 
			GetWorld()->SpawnActor<AProjectileActor>(ProjectileClass, SocketLocation, OwnerAvatarActor->GetActorRotation(), SpawnParams);
		if (ProjectileActor)
		{
			ProjectileActor->ShootProjectile(ShootProjectileSpeed, ShootProjectileRange, nullptr,
				GetOwnerTeamId(), MakeOutgoingGameplayEffectSpec(ProjectileHitEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)));
		}
	}
}
