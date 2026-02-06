// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MinionBarrack.h"

#include "Minion.h"
#include "GameFramework/PlayerStart.h"

// Sets default values
AMinionBarrack::AMinionBarrack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMinionBarrack::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(SpawnIntervalTimerHandle, this, &AMinionBarrack::SpawnNewGroup, 
			GroupSpawnInterval, true);
		SpawnNewMinions(5);
	}
}

// Called every frame
void AMinionBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

const APlayerStart* AMinionBarrack::GetNextSpawnSpot()
{
	if (SpawnSpots.Num() == 0) return nullptr;
	++NextSpawnSpotIndex; //索引递增
	if (NextSpawnSpotIndex >= SpawnSpots.Num()) NextSpawnSpotIndex = 0; //数组已满重置索引
	return SpawnSpots[NextSpawnSpotIndex]; //返回索引
}

void AMinionBarrack::SpawnNewMinions(int Amount)
{
	for (int i = 0; i < Amount; i++)
	{
		FTransform SpawnTransform = GetActorTransform();
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}
		AMinion* NewMinion = GetWorld()->SpawnActorDeferred<AMinion>(MinionClass, SpawnTransform, this, 
			nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn); //生成兵营
		NewMinion->SetGenericTeamId(BarrackTeamId); //设置team id
		NewMinion->FinishSpawning(SpawnTransform); //完成生成
		NewMinion->SetGoal(Goal); //设置目标
		MinionPool.Add(NewMinion);
	}
}

void AMinionBarrack::SpawnNewGroup()
{
	int i = MinionPerGroup;

	while (i > 0)
	{
		FTransform SpawnTransform = GetActorTransform();
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}
		
		AMinion* NextAvailableMinion = GetNextAvailableMinion();
		if (!NextAvailableMinion) 
			break;
		
		NextAvailableMinion->SetActorTransform(SpawnTransform);
		NextAvailableMinion->Activate();
		--i;
	}
	SpawnNewMinions(1);
}

AMinion* AMinionBarrack::GetNextAvailableMinion() const
{
	for (AMinion* Minion : MinionPool)
	{
		if (!Minion->IsActive()) return Minion;
	}
	return nullptr;
}

