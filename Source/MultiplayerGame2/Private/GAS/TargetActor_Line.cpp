// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActor_Line.h"
#include "Components/SphereComponent.h"
#include "MultiplayerGame2/MultiplayerGame2.h"
#include "NiagaraComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ATargetActor_Line::ATargetActor_Line()
{
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Component");
	SetRootComponent(RootComp);
	
	TargetEndDetectionSphere = CreateDefaultSubobject<USphereComponent>("Target End Detection Sphere");
	TargetEndDetectionSphere->SetupAttachment(GetRootComponent());
	TargetEndDetectionSphere->SetCollisionResponseToChannel(ECC_SpringArm, ECR_Ignore);
	
	LazerVFX = CreateDefaultSubobject<UNiagaraComponent>("Lazer VFX");
	LazerVFX->SetupAttachment(GetRootComponent());
	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true; //仅在server处理逻辑，在client显示模拟效果
	
	AvatarActor = nullptr;
}

void ATargetActor_Line::ConfigureTargetSetting(float NewTargetRange, float NewDetectionCylinderRadius,
	float NewTargetingInterval, FGenericTeamId OwnerTeamId, bool bShouldDrawDebug)
{
	TargetRange = NewTargetRange;
	DetectionCylinderRadius = NewDetectionCylinderRadius;
	TargetingInterval = NewTargetingInterval;
	SetGenericTeamId(OwnerTeamId);
	bDrawDebug = bShouldDrawDebug;
}

void ATargetActor_Line::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

void ATargetActor_Line::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATargetActor_Line, TeamId);
	DOREPLIFETIME(ATargetActor_Line, TargetRange);
	DOREPLIFETIME(ATargetActor_Line, DetectionCylinderRadius);
	DOREPLIFETIME(ATargetActor_Line, AvatarActor);
}

void ATargetActor_Line::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	if (!OwningAbility) return;
	AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(PeriodicalTargetingTimer, this, &ATargetActor_Line::DoTargetCheckAndReport, TargetingInterval, true);
	}
}

void ATargetActor_Line::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateTargetTrace();
}

void ATargetActor_Line::DoTargetCheckAndReport()
{
	
}

void ATargetActor_Line::UpdateTargetTrace()
{
	FVector ViewLocation = GetActorLocation();
	FRotator ViewRotation = GetActorRotation();
	if (AvatarActor)
	{
		AvatarActor->GetActorEyesViewPoint(ViewLocation, ViewRotation);
	}
	FVector LookEndPoint = ViewLocation + ViewRotation.Vector() * 100000;
	FRotator LookEndRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LookEndPoint);
	SetActorRotation(LookEndRotation);
	FVector SweepEndLocation = GetActorLocation() + LookEndRotation.Vector() * TargetRange;
	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(AvatarActor);
	CollisionQueryParams.AddIgnoredActor(this);
	FCollisionResponseParams CollisionResponseParams(ECR_Overlap);
	GetWorld()->SweepMultiByChannel(HitResults, GetActorLocation(), SweepEndLocation, FQuat::Identity, 
		ECC_WorldDynamic, FCollisionShape::MakeSphere(DetectionCylinderRadius), CollisionQueryParams, CollisionResponseParams);
	FVector LineEndLocation = SweepEndLocation;
	float LineLength = TargetRange;
	for (FHitResult& HitResult : HitResults)
	{
		if (HitResult.GetActor())
		{
			if (GetTeamAttitudeTowards(*HitResult.GetActor()) != ETeamAttitude::Friendly)
			{
				LineEndLocation = HitResult.ImpactPoint;
				LineLength = FVector::Distance(GetActorLocation(), LineEndLocation);
				break;
			}
		}
	}
	TargetEndDetectionSphere->SetWorldLocation(LineEndLocation);
	if (LazerVFX)
	{
		LazerVFX->SetVariableFloat(LazerFXLengthParamName, LineLength / 100.f);
	}
}
