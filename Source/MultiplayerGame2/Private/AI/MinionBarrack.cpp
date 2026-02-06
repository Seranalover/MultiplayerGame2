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
	SpawnNewMinions(5);
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
		MinionPool.Add(NewMinion);
	}
}

