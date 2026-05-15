// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TA_BlackHole.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

ATA_BlackHole::ATA_BlackHole()
{
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Comp");
	SetRootComponent(RootComp);
	
	DetectionSphereComp = CreateDefaultSubobject<USphereComponent>("Detection Sphere Comp");
	DetectionSphereComp->SetupAttachment(GetRootComponent());
	DetectionSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	DetectionSphereComp->OnComponentBeginOverlap.AddDynamic(this, &ATA_BlackHole::ActorInBlackHoleRange);
	DetectionSphereComp->OnComponentEndOverlap.AddDynamic(this, &ATA_BlackHole::ActorLeftBlackHoleRange);
	
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;
	PrimaryActorTick.bCanEverTick = true;
	
	VFXComp = CreateDefaultSubobject<UParticleSystemComponent>("VFX Comp");
	VFXComp->SetupAttachment(GetRootComponent());
}

void ATA_BlackHole::ConfigureBlackHole(float InBlackHoleRange, float InPullSpeed, float InBlackHoleDuration, const FGenericTeamId& InTeamId)
{
	PullSpeed = InPullSpeed;
	DetectionSphereComp->SetSphereRadius(InBlackHoleRange);
	SetGenericTeamId(InTeamId);
	BlackHoleDuration = InBlackHoleDuration;
	BlackHoleDuration = InBlackHoleRange;
}

void ATA_BlackHole::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	TeamId = TeamID;
}

void ATA_BlackHole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATA_BlackHole, TeamId);
	DOREPLIFETIME_CONDITION_NOTIFY(ATA_BlackHole, BlackHoleRange, COND_None, REPNOTIFY_Always);
}

void ATA_BlackHole::OnRep_BlackHoleRange()
{
	DetectionSphereComp->SetSphereRadius(BlackHoleRange);
}

void ATA_BlackHole::ActorInBlackHoleRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ATA_BlackHole::ActorLeftBlackHoleRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
