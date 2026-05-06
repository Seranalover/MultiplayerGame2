// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_Line.generated.h"

/**
 * 目标选择器 - 线性 基础类
 */
UCLASS()
class ATargetActor_Line : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	ATargetActor_Line();
	void ConfigureTargetSetting(
		float NewTargetRange, //瞄准范围，即射程
		float NewDetectionCylinderRadius, //目标半径 
		float NewTargetingInterval, //检测时间间隔
		FGenericTeamId OwnerTeamId,
		bool bShouldDrawDebug
	);
	
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const { return TeamId; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(Replicated)
	float TargetRange;
	
	UPROPERTY(Replicated)
	float DetectionCylinderRadius;
	
	UPROPERTY()
	float TargetingInterval;
	
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;
	
	UPROPERTY()
	bool bDrawDebug;
	
	UPROPERTY(Replicated)
	const AActor* AvatarActor;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Component")	
	class USceneComponent* RootComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	class UNiagaraComponent* LazerVFX;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	class USphereComponent* TargetEndDetectionSphere;
};
