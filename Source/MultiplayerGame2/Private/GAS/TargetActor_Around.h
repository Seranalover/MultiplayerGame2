// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_Around.generated.h"

/**
 * 目标选择器 - 身边周围
 */
UCLASS()
class ATargetActor_Around : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	ATargetActor_Around();
	void ConfigureDetection(float DetectionRadius, const FGenericTeamId& InTeamId, const FGameplayTag InLocalGameplayCueTag);
	
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const { return TeamId; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Comp")
	class USceneComponent* RootComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Targeting")
	class USphereComponent* DetectionSphere;
	
	UPROPERTY(ReplicatedUsing=OnRep_TargetDetectionRadiusReplicated)
	float TargetDetectRadius;
	
	UFUNCTION()
	void OnRep_TargetDetectionRadiusReplicated();
	
	UPROPERTY(Replicated)
	FGameplayTag LocalGamePlayCueTag;
	
	UFUNCTION()
	void ActorInDetectionRange(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
