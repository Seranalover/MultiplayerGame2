// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/ProjectileActor.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AProjectileActor::AProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>("Root Component");
	SetRootComponent(RootComp);
	bReplicates = true; //可复制
}

void AProjectileActor::ShootProjectile(float InSpeed, float InMaxDistance, const AActor* InTarget,
	FGenericTeamId InTeamId, FGameplayEffectSpecHandle InHitSpecHandle)
{
	Target = InTarget;
	ProjectileSpeed = InSpeed;
	FRotator OwnerViewRot = GetActorRotation();
	SetGenericTeamId(InTeamId);
	if (GetOwner())
	{
		FVector OwnerViewLoc;
		GetOwner()->GetActorEyesViewPoint(OwnerViewLoc, OwnerViewRot);
	}
	MoveDirection = OwnerViewRot.Vector();
	HitEffectSpecHandle = InHitSpecHandle;
	float TravelMaxTime = InMaxDistance / InSpeed;
	GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AProjectileActor::TravelMaxDistanceReached, TravelMaxTime);
}

void AProjectileActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectileActor, MoveDirection);
	DOREPLIFETIME(AProjectileActor, TeamId);
	DOREPLIFETIME(AProjectileActor, ProjectileSpeed);
}

void AProjectileActor::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

// Called when the game starts or when spawned
void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		if (Target)
		{
			MoveDirection = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal(); //自动向client同步，无需手动同步
		}
	}
	SetActorLocation(GetActorLocation() + MoveDirection * ProjectileSpeed * DeltaTime);
}

void AProjectileActor::TravelMaxDistanceReached()
{
	Destroy();
}

