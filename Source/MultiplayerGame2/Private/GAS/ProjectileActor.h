// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

/**
 * 投射物/子弹类
 */
UCLASS()
class AProjectileActor : public AActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileActor();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void ShootProjectile(
		float InSpeed,
		float InMaxDistance,
		const AActor* InTarget, //射击目标
		FGenericTeamId InTeamId,
		FGameplayEffectSpecHandle InHitSpecHandle
	);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const { return TeamId; }
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override; //与其他actor或component重叠时触发
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Cue")
	FGameplayTag HitGameplayCueTag;
	
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;
	
	UPROPERTY(Replicated)
	FVector MoveDirection; //射击方向
	
	UPROPERTY(Replicated)
	float ProjectileSpeed;
	
	UPROPERTY()
	const AActor* Target;
	
	FGameplayEffectSpecHandle HitEffectSpecHandle;
	FTimerHandle ShootTimerHandle;
	
	void TravelMaxDistanceReached(); //子弹达到最大距离时的处理，例如销毁
	void SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
