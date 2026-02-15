// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActor_GroundPick.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "Components/DecalComponent.h"
#include "Engine/OverlapResult.h"
#include "GenericTeamAgentInterface.h"
#include "MultiplayerGame2/MultiplayerGame2.h"

ATargetActor_GroundPick::ATargetActor_GroundPick()
{
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootS Component"));
	DecalComponent = CreateDefaultSubobject<UDecalComponent>("Decal Component");
	DecalComponent->SetupAttachment(RootComponent);
}

void ATargetActor_GroundPick::SetTargetAreaRadius(float NewRadius)
{
	TargetAreaRadius = NewRadius;
	DecalComponent->DecalSize = FVector(NewRadius);
}

void ATargetActor_GroundPick::SetTargetTraceRange(float NewRange)
{
	TargetRange = NewRange;
}

void ATargetActor_GroundPick::ConfirmTargetingAndContinue()
{
	TArray<FOverlapResult> OverlapResults; //重叠结果
	FCollisionObjectQueryParams CollisionParams; //碰撞参数
	CollisionParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionShape CollisionShape; //碰撞形状
	CollisionShape.SetSphere(TargetAreaRadius);
	GetWorld()->OverlapMultiByObjectType(OverlapResults, GetActorLocation(), FQuat::Identity, CollisionParams, CollisionShape); //碰撞检测
	TSet<AActor*> TargetActors; //命中对象集合
	IGenericTeamAgentInterface* TeamAgentInterface = nullptr;
	if (OwningAbility)
	{
		TeamAgentInterface = Cast<IGenericTeamAgentInterface>(OwningAbility->GetAvatarActorFromActorInfo());
	}
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (TeamAgentInterface && TeamAgentInterface->GetTeamAttitudeTowards(*OverlapResult.GetActor()) == ETeamAttitude::Friendly && !bShouldTargetFriendly) //排除友方单位
			continue;
		if (TeamAgentInterface && TeamAgentInterface->GetTeamAttitudeTowards(*OverlapResult.GetActor()) == ETeamAttitude::Hostile && !bShouldTargetEnemy) //排除特定敌方单位
			continue;
		TargetActors.Add(OverlapResult.GetActor());
	}
	FGameplayAbilityTargetDataHandle TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(TargetActors.Array(), false); //选中目标
	FGameplayAbilityTargetData_SingleTargetHit* HitLocation = new FGameplayAbilityTargetData_SingleTargetHit; //获得单个目标对象
	HitLocation->HitResult.ImpactPoint = GetActorLocation(); //获得目标对象位置，用于生成粒子特效
	TargetData.Add(HitLocation); //将位置信息记入TargetData
	TargetDataReadyDelegate.Broadcast(TargetData); //广播事件，触发GA_GroundBlast类中ValidData函数
}

void ATargetActor_GroundPick::SetTargetOptions(bool bTargetFriendly, bool bTargetEnemy)
{
	bShouldTargetEnemy = bTargetEnemy;
	bShouldTargetFriendly = bTargetFriendly;
}

void ATargetActor_GroundPick::Tick(float DeltaTime)
{
	//client and local player only
	Super::Tick(DeltaTime);
	if (PrimaryPC && PrimaryPC->IsLocalPlayerController()) //PrimaryPC是父类的基础变量，代表玩家控制器的本地实例
	{
		SetActorLocation(GetTargetActorLocation());
	}
}

FVector ATargetActor_GroundPick::GetTargetActorLocation() const
{
	if (!PrimaryPC || !PrimaryPC->IsLocalPlayerController()) return GetActorLocation();
	
	FHitResult TraceResult; //扫描结果
	FVector ViewLocation; //玩家视线Aim位置
	FRotator ViewRotation; //视线旋转
	PrimaryPC->GetPlayerViewPoint(ViewLocation, ViewRotation); //获取当前ViewLocation和ViewRotation
	FVector TraceEnd = ViewLocation + ViewRotation.Vector() * TargetRange; //trace终点位置
	GetWorld()->LineTraceSingleByChannel(TraceResult, ViewLocation, TraceEnd, ECC_Target); //射线追踪玩家Aim位置，ECC_Target是DefaultEngine.ini新建的检测通道，并且在MultiplayerGame2.h中重命名
	//如果没检测到结果，可能是看向空中，从空中的Aim位置投影到地面，再次进行一次射线检测
	if (!TraceResult.bBlockingHit)
		GetWorld()->LineTraceSingleByChannel(TraceResult, ViewLocation, TraceEnd + FVector::DownVector * TNumericLimits<float>::Max(), ECC_Target);
	//如果还没检测到结果，直接返回
	if (!TraceResult.bBlockingHit)
		return GetActorLocation();
	//绘制debug范围
	if (bShouldDrawDebug)
		DrawDebugSphere(GetWorld(), TraceResult.ImpactPoint, TargetAreaRadius, 32, FColor::Red);
	return TraceResult.ImpactPoint; //返回trace的命中结果
}
