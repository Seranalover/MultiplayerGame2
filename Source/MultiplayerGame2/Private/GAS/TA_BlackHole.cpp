// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TA_BlackHole.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"

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

void ATA_BlackHole::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(BlackHoleDurationTimerHandle, this, &ATA_BlackHole::StopBlackHole, BlackHoleDuration);
	}
}

void ATA_BlackHole::Tick(float DeltaSeconds)
{
	if (HasAuthority())
	{
		for (TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap)
		{
			AActor* Target = TargetPair.Key;
			UNiagaraComponent* NiagaraComponent = TargetPair.Value;
			FVector PullDirection = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
			Target->SetActorLocation(Target->GetActorLocation() + PullDirection * PullSpeed * DeltaSeconds);
			if (NiagaraComponent)
			{
				NiagaraComponent->SetVariablePosition(BlackHoleVFXOriginVariableName, VFXComp->GetComponentLocation());
			}
		}
	}
}

void ATA_BlackHole::ConfirmTargetingAndContinue()
{
	StopBlackHole();
}

void ATA_BlackHole::CancelTargeting()
{
	StopBlackHole();
	Super::CancelTargeting();
}

void ATA_BlackHole::OnRep_BlackHoleRange()
{
	DetectionSphereComp->SetSphereRadius(BlackHoleRange);
}

void ATA_BlackHole::ActorInBlackHoleRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TryAddTarget(OtherActor);
}

void ATA_BlackHole::ActorLeftBlackHoleRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	RemoveTarget(OtherActor);
}

void ATA_BlackHole::TryAddTarget(AActor* OtherTarget)
{
	if (!OtherTarget || ActorsInRangeMap.Contains(OtherTarget)) return;
	if (GetTeamAttitudeTowards(*OtherTarget) != ETeamAttitude::Hostile) return;
	
	UNiagaraComponent* NiagaraComponent = nullptr;
	if (BlackHoleLinkVFX)
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(BlackHoleLinkVFX, OtherTarget->GetRootComponent(),
			NAME_None, FVector::Zero(), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
		if (NiagaraComponent)
		{
			NiagaraComponent->SetVariablePosition(BlackHoleVFXOriginVariableName, VFXComp->GetComponentLocation());
		}
	}
	ActorsInRangeMap.Add(OtherTarget, NiagaraComponent);
}

void ATA_BlackHole::RemoveTarget(AActor* OtherTarget)
{
	if (!OtherTarget) return;
	if (ActorsInRangeMap.Contains(OtherTarget))
	{
		UNiagaraComponent* VfxComponent;
		ActorsInRangeMap.RemoveAndCopyValue(OtherTarget, VfxComponent);
		if (IsValid(VfxComponent))
		{
			VfxComponent->DestroyComponent();
		}
		
	}
}

void ATA_BlackHole::StopBlackHole()
{
	TArray<TWeakObjectPtr<AActor>> FinalTargets;
	for (TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap)
	{
		FinalTargets.Add(TargetPair.Key);
		UNiagaraComponent* NiagaraComponent = TargetPair.Value;
		if (NiagaraComponent)
		{
			NiagaraComponent->DestroyComponent();
		}
	}
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	
	FGameplayAbilityTargetData_ActorArray* TargetActorArray = new FGameplayAbilityTargetData_ActorArray;
	TargetActorArray->SetActors(FinalTargets);
	TargetDataHandle.Add(TargetActorArray);
	
	FGameplayAbilityTargetData_SingleTargetHit* BlowupLocation = new FGameplayAbilityTargetData_SingleTargetHit;
	BlowupLocation->HitResult.ImpactPoint = GetActorLocation();
	TargetDataHandle.Add(BlowupLocation);
	
	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}
