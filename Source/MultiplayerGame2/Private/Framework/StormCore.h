// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StormCore.generated.h"

/**
 * 风暴核心
 */
UCLASS()
class AStormCore : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStormCore();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	class USphereComponent* InfluenceRange; //影响范围
	
	int TeamOneInfluenceCount = 0; //队伍1数量
	int TeamTwoInfluenceCount = 0; //队伍2数量
	float TeamWeight = 0.0f; //队伍数量统计
	
	UFUNCTION()
	void NewInfluencerInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); //单位进入影响范围
	
	UFUNCTION()
	void InfluencerLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex); //单位离开影响范围
	
	void UpdateTeamWeight();
};
