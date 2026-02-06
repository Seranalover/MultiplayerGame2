// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GA_PassiveLaunched.h"
#include "Kismet/KismetSystemLibrary.h"

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
	
	for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : TargetDataHandle.Data)
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

ACharacter* UCGameplayAbility::GetOwningAvatarCharacter()
{
	if (!CharacterRef)
		CharacterRef = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	return CharacterRef;
}
