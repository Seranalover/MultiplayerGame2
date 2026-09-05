// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CAbilitySystemStatics.h"
#include "GA_PassiveLaunched.h"
#include "Kismet/KismetSystemLibrary.h"

UCGameplayAbility::UCGameplayAbility()
{
	ActivationBlockedTags.AddTag(UCAbilitySystemStatics::GetStunStatTag()); //默认具有stun tag时停止能力
}

bool UCGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	if (AbilitySpec && AbilitySpec->Level <= 0) return false; //未学习的技能无法施放
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

class UAnimInstance* UCGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMeshComponent = GetOwningComponentFromActorInfo(); //获得骨骼组件
	if (OwnerSkeletalMeshComponent) return OwnerSkeletalMeshComponent->GetAnimInstance(); //返回动画实例
	return nullptr;
}

TArray<FHitResult> UCGameplayAbility::GetHitResultsFromSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius, ETeamAttitude::Type TargetTeam,
	bool bDrawDebug, bool bIgnoreSelf) const
{
	TArray<FHitResult> OutResults; //返回的命中结果
	TSet<AActor*> HitActors; //已命中过的actor，避免重复命中同一个actor
	
	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	
	for (const TSharedPtr<FGameplayAbilityTargetData>& TargetData : TargetDataHandle.Data)
	{
		FVector SourceLocation = TargetData->GetOrigin().GetTranslation(); //扫描开始位置
		FVector TargetLocation = TargetData->GetEndPoint(); //扫描结束位置
		
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); //Trace的对象类型
		
		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreSelf) ActorsToIgnore.Add(GetAvatarActorFromActorInfo()); //忽略的actor数组
		
		EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None; //是否绘制debug轨迹？
		
		TArray<FHitResult> HitResults; //命中结果数组
		
		//执行球体检测
		UKismetSystemLibrary::SphereTraceMultiForObjects(this, SourceLocation, TargetLocation, 
			SphereSweepRadius, ObjectTypes, false, ActorsToIgnore, DrawDebugTrace, HitResults, false);
		
		for (const FHitResult& HitResult : HitResults)
		{
			if (HitActors.Contains(HitResult.GetActor())) 
				continue; //已命中过actor，跳过
			
			if (OwnerTeamInterface)
			{
				ETeamAttitude::Type OtherActorTeamAttitude = OwnerTeamInterface->GetTeamAttitudeTowards(*HitResult.GetActor());
				if (OtherActorTeamAttitude != TargetTeam)
					continue; //命中非敌方，跳过
			}
			
			HitActors.Add(HitResult.GetActor()); 
			OutResults.Add(HitResult);
		}
	}
	
	return OutResults;
}

void UCGameplayAbility::PushSelf(const FVector& PushVelocity)
{
	ACharacter* OwningAvatarCharacter = GetOwningAvatarCharacter();
	if (OwningAvatarCharacter)
	{
		OwningAvatarCharacter->LaunchCharacter(PushVelocity, true, true); //将自身推向某个方向
	}
}

void UCGameplayAbility::PushTarget(AActor* Target, const FVector& PushVelocity)
{
	if (!Target) return;
	FGameplayEventData PushEventData;
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit; //被击中的目标
	FHitResult HitResult;
	HitResult.ImpactNormal = PushVelocity; //将推动方向存入HitResult
	TargetData->HitResult = HitResult; //将HitResult存入TargetData
	PushEventData.TargetData.Add(TargetData); //将TargetData存入事件数据
	
	//将推动事件数据传递给GA_PassiveLaunched技能类
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, UGA_PassiveLaunched::GetLaunchedAbilityTriggerTag(), PushEventData);
}

void UCGameplayAbility::PushTargets(const TArray<AActor*>& Targets, const FVector& PushVelocity)
{
	for (AActor* Target : Targets)
	{
		PushTarget(Target, PushVelocity);
	}
}

void UCGameplayAbility::PushTargets(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVelocity)
{
	TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargets(Targets, PushVelocity);
}

void UCGameplayAbility::PushTargetsFromLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& FromLocation, float PushSpeed)
{
	TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargetsFromLocation(Targets, FromLocation, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromLocation(const TArray<AActor*>& Targets, const FVector& FromLocation, float PushSpeed)
{
	for (AActor* Target : Targets)
	{
		FVector PushDir = Target->GetActorLocation() - FromLocation;
		PushDir.Z = 0;
		PushDir.Normalize();
		
		PushTarget(Target, PushDir * PushSpeed);
	}
}

void UCGameplayAbility::PushTargetsFromOwnerLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float PushSpeed)
{
	TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargetsFromOwnerLocation(TargetActors, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromOwnerLocation(const TArray<AActor*>& Targets, float PushSpeed)
{
	AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
	if (!OwnerAvatarActor) return;
	
	FVector OwnerLocation = OwnerAvatarActor->GetActorLocation();
	PushTargetsFromLocation(Targets, OwnerLocation, PushSpeed);
}

ACharacter* UCGameplayAbility::GetOwningAvatarCharacter()
{
	if (!CharacterRef)
		CharacterRef = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	return CharacterRef;
}

void UCGameplayAbility::ApplyGameplayEffectToHitResult(const FHitResult& HitResult,
	TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect, Level); //设置当前技能等级的effect spec handle
		
	//将HitResults信息传递到Context里，以便于在GameplayCue蓝图中调用
	FGameplayEffectContextHandle ContextHandle = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
	ContextHandle.AddHitResult(HitResult);
	EffectSpecHandle.Data->SetContext(ContextHandle);
		
	//使用handle应用攻击效果
	ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), CurrentActorInfo, CurrentActivationInfo,
		EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor()));
}

void UCGameplayAbility::PlayMontageLocally(UAnimMontage* MontageToPlay)
{
	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (AnimInstance && !AnimInstance->Montage_IsPlaying(MontageToPlay))
		AnimInstance->Montage_Play(MontageToPlay);
}

void UCGameplayAbility::StopMontageAfterCurrentSection(UAnimMontage* MontageToStop)
{
	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (AnimInstance)
	{
		FName CurrentSection = AnimInstance->Montage_GetCurrentSection(MontageToStop);
		if (CurrentSection != NAME_None)
			AnimInstance->Montage_SetNextSection(CurrentSection, NAME_None, MontageToStop);
	}
}

FGenericTeamId UCGameplayAbility::GetOwnerTeamId() const
{
	IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	if (TeamAgentInterface)
	{
		return TeamAgentInterface->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

AActor* UCGameplayAbility::GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const
{
	AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
	if (OwnerAvatarActor)
	{
		FVector Location;
		FRotator Rotation;
		OwnerAvatarActor->GetActorEyesViewPoint(Location, Rotation);
		FVector AimEnd = Location + Rotation.Vector() * AimDistance;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(OwnerAvatarActor);
		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
		if (ShouldDrawDebug())
		{
			DrawDebugLine(GetWorld(), Location, AimEnd, FColor::Red, false, 2.f, 0U, 3.f);
		}
		TArray<FHitResult> HitResults;
		if (GetWorld()->LineTraceMultiByObjectType(HitResults, Location, AimEnd, ObjectParams, CollisionParams))
		{
			for (FHitResult& HitResult : HitResults)
			{
				if (IsActorTeamAttitude(HitResult.GetActor(), TeamAttitude))
					return HitResult.GetActor();
			}
		}
	}
	return nullptr;
}

bool UCGameplayAbility::IsActorTeamAttitude(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const
{
	if (!OtherActor) return false;
	IGenericTeamAgentInterface* OwnerTeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	if (OwnerTeamAgentInterface)
	{
		return OwnerTeamAgentInterface->GetTeamAttitudeTowards(*OtherActor) == TeamAttitude;
	}
	return false;
}

void UCGameplayAbility::SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (OwnerASC)
	{
		OwnerASC->HandleGameplayEvent(EventTag, &EventData);
	}
}
