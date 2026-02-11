// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActor_GroundPick.h"
#include "MultiplayerGame2/MultiplayerGame2.h"

ATargetActor_GroundPick::ATargetActor_GroundPick()
{
	PrimaryActorTick.bCanEverTick = true;
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
	
	return TraceResult.ImpactPoint; //返回trace的命中结果
}
