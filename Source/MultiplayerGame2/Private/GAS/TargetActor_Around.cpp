// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActor_Around.h"

#include "CAbilitySystemStatics.h"
#include "Abilities/GameplayAbility.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


ATargetActor_Around::ATargetActor_Around()
{
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Comp");
	SetRootComponent(RootComp);
	
	DetectionSphere = CreateDefaultSubobject<USphereComponent>("Detection Sphere");
	DetectionSphere->SetupAttachment(GetRootComponent());
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATargetActor_Around::ActorInDetectionRange);
	
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;
}

void ATargetActor_Around::ConfigureDetection(float DetectionRadius, const FGenericTeamId& InTeamId,
	const FGameplayTag InLocalGameplayCueTag)
{
	SetGenericTeamId(InTeamId);
	DetectionSphere->SetSphereRadius(DetectionRadius);
	TargetDetectRadius = DetectionRadius;
	LocalGamePlayCueTag = InLocalGameplayCueTag;
}

void ATargetActor_Around::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

void ATargetActor_Around::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ATargetActor_Around, TeamId);
	DOREPLIFETIME(ATargetActor_Around, LocalGamePlayCueTag);
	DOREPLIFETIME(ATargetActor_Around, TargetDetectRadius);
}

void ATargetActor_Around::OnRep_TargetDetectionRadiusReplicated()
{
	DetectionSphere->SetSphereRadius(TargetDetectRadius);
}

void ATargetActor_Around::ActorInDetectionRange(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;
	
	AActor* AvatarActor = nullptr;
	if (OwningAbility)
		AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
	
	if (OtherActor == AvatarActor)
		return;
	
	if (OtherActor == this)
		return;
	
	if (GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile)
		return;
	
	if (HasAuthority())
	{
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		FGameplayAbilityTargetData_ActorArray* ActorArray = new FGameplayAbilityTargetData_ActorArray;
		ActorArray->SetActors(TArray<TWeakObjectPtr<AActor>>{OtherActor});
		
		TargetDataHandle.Add(ActorArray);
		TargetDataReadyDelegate.Broadcast(TargetDataHandle);
	}
	
	FHitResult HitResult;
	HitResult.ImpactPoint = OtherActor->GetActorLocation();
	HitResult.ImpactNormal = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	UCAbilitySystemStatics::SendLocalGameplayCue(OtherActor, HitResult, LocalGamePlayCueTag);
}
