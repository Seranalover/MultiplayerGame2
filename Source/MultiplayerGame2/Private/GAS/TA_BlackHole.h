// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TA_BlackHole.generated.h"

/**
 * 黑洞技能目标选择器
 * 本质上属于Actor
 * 具备一定程度的网络同步能力，并非完全同步
 */
UCLASS()
class ATA_BlackHole : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	ATA_BlackHole();
	void ConfigureBlackHole(float InBlackHoleRange, float InPullSpeed, float InBlackHoleDuration, const FGenericTeamId& InTeamId);
	
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const { return TeamId; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void Tick(float DeltaSeconds) override;
	
private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;
	
	float PullSpeed;
	float BlackHoleDuration;
	FTimerHandle BlackHoleDurationTimerHandle;
	
	UPROPERTY(ReplicatedUsing = OnRep_BlackHoleRange)
	float BlackHoleRange;
	
	UPROPERTY(EditDefaultsOnly, Category="VFX")
	FName BlackHoleVFXOriginVariableName = "Origin";
	
	UPROPERTY(EditDefaultsOnly, Category="VFX")
	class UNiagaraSystem* BlackHoleLinkVFX;
	
	UFUNCTION()
	void OnRep_BlackHoleRange();
	
	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	class USceneComponent* RootComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	class USphereComponent* DetectionSphereComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	class UParticleSystemComponent* VFXComp;
	
	UFUNCTION()
	void ActorInBlackHoleRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION()
	void ActorLeftBlackHoleRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void TryAddTarget(AActor* OtherTarget);
	void RemoveTarget(AActor* OtherTarget);
	
	TMap<AActor*, class UNiagaraComponent*> ActorsInRangeMap;
	
	void StopBlackHole();
};
