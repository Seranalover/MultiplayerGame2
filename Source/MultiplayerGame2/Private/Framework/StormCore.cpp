// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/StormCore.h"
#include "GenericTeamAgentInterface.h"
#include "Components/SphereComponent.h"

// Sets default values
AStormCore::AStormCore()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InfluenceRange = CreateDefaultSubobject<USphereComponent>("Influence Range");
	InfluenceRange->SetupAttachment(GetRootComponent());
	InfluenceRange->OnComponentBeginOverlap.AddDynamic(this, &AStormCore::NewInfluencerInRange); //单位进入影响范围
	InfluenceRange->OnComponentEndOverlap.AddDynamic(this, &AStormCore::InfluencerLeftRange); //单位离开影响范围
}

// Called when the game starts or when spawned
void AStormCore::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStormCore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AStormCore::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AStormCore::NewInfluencerInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IGenericTeamAgentInterface* OtherTeamAgentInterface = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (OtherTeamAgentInterface)
	{
		if (OtherTeamAgentInterface->GetGenericTeamId().GetId() == 0)
			TeamOneInfluenceCount++;
		else if (OtherTeamAgentInterface->GetGenericTeamId().GetId() == 1)
			TeamTwoInfluenceCount++;
		UpdateTeamWeight();
	}
}

void AStormCore::InfluencerLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IGenericTeamAgentInterface* OtherTeamAgentInterface = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (OtherTeamAgentInterface)
	{
		if (OtherTeamAgentInterface->GetGenericTeamId().GetId() == 0)
		{
			TeamOneInfluenceCount--;
			if (TeamOneInfluenceCount < 0)
				TeamOneInfluenceCount = 0;
		}
		else if (OtherTeamAgentInterface->GetGenericTeamId().GetId() == 1)
		{
			TeamTwoInfluenceCount--;
			if (TeamTwoInfluenceCount < 0)
				TeamTwoInfluenceCount = 0;
		}
		UpdateTeamWeight();
	}
}

void AStormCore::UpdateTeamWeight()
{
	if (TeamOneInfluenceCount == TeamTwoInfluenceCount)
		TeamWeight = 0.f;
	else
	{
		float TeamOffset = TeamOneInfluenceCount - TeamTwoInfluenceCount;
		float TeamTotal = TeamOneInfluenceCount + TeamTwoInfluenceCount;
		TeamWeight = TeamOffset / TeamTotal;
	}
	UE_LOG(LogTemp, Warning, TEXT("Team one count: %d, Team two count: %d, Weight: %f"), TeamOneInfluenceCount, TeamTwoInfluenceCount, TeamWeight);
}

