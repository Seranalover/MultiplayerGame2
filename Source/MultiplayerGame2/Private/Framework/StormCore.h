// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StormCore.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGoalReachedDelegate, AActor* /*ViewTarget*/, int /*WiningTeam*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTeamInfluencerCountUpdatedDelegate, int /*TeamOneInfluencerCount*/, int /*TeamTwoInfluencerCount*/);
/**
 * 风暴核心
 */
UCLASS()
class AStormCore : public ACharacter
{
	GENERATED_BODY()

public:
	FOnGoalReachedDelegate OnGoalReachedDelegate;
	FOnTeamInfluencerCountUpdatedDelegate OnTeamInfluencerCountUpdatedDelegate;
	
	// Sets default values for this character's properties
	AStormCore();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* ExpandMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* CaptureMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Move")
	float InfluenceRadius = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Move")
	float MaxMoveSpeed = 500.f;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	class USphereComponent* InfluenceRange; //影响范围
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	class UDecalComponent* GroundDecalComponent; //影响范围贴花效果
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	class UCameraComponent* ViewCamera; //摄像机组件，用于胜利结算界面
	
	int TeamOneInfluenceCount = 0; //队伍1数量
	int TeamTwoInfluenceCount = 0; //队伍2数量
	float TeamWeight = 0.0f; //队伍数量统计
	
	UPROPERTY(EditAnywhere, Category="Team")
	AActor* TeamOneGoal;
	UPROPERTY(EditAnywhere, Category="Team")
	AActor* TeamTwoGoal;
	UPROPERTY(EditAnywhere, Category="Team")
	AActor* TeamOneCore;
	UPROPERTY(EditAnywhere, Category="Team")
	AActor* TeamTwoCore;
	
	UPROPERTY(ReplicatedUsing=OnRep_CoreToCapture)
	AActor* CoreToCapture;
	
	float CoreCaptureSpeed = 0.f;
	
	UPROPERTY()
	class AAIController* OwnerAIC;
	
	UFUNCTION()
	void NewInfluencerInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); //单位进入影响范围
	
	UFUNCTION()
	void InfluencerLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex); //单位离开影响范围
	
	void UpdateTeamWeight();
	void UpdateGoal();
	
	UFUNCTION()
	void OnRep_CoreToCapture();
	
	void GoalReached(int WiningTeam);
	void CaptureCore();
	void ExpandFinished();
};
