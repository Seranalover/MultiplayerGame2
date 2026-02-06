// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "MinionBarrack.generated.h"

/**
 * 兵营类，用于生成小兵
 */
UCLASS()
class AMinionBarrack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMinionBarrack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FGenericTeamId BarrackTeamId;
	
	UPROPERTY()
	TArray<class AMinion*> MinionPool;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class AMinion> MinionClass;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<class APlayerStart*> SpawnSpots; //出生点
	
	int NextSpawnSpotIndex = -1; //下一个出生点索引
	
	FTimerHandle SpawnIntervalTimerHandle; //生成间隔计时器句柄
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	float GroupSpawnInterval = 10.f;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	int MinionPerGroup = 3; 
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	AActor* Goal; //目标
	
	const APlayerStart* GetNextSpawnSpot();
	void SpawnNewMinions(int Amount); //生成小兵
	void SpawnNewGroup();
	AMinion* GetNextAvailableMinion() const; 
};
