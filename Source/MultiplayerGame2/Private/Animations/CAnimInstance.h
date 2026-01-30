// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CAnimInstance.generated.h"

/**
 * 动画蓝图基类
 */
UCLASS()
class UCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// the below functions are the native overrides for each phase
	// Native initialization override point
	virtual void NativeInitializeAnimation() override;
	// Native update override point. It is usually a good idea to simply gather data in this step and 
	// for the bulk of the work to be done in NativeThreadSafeUpdateAnimation.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	// Native thread safe update override point. Executed on a worker thread just prior to graph update 
	// for linked anim instances, only called when the hosting node(s) are relevant
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	
	// getSpeed()
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsMoving() const { return Speed != 0.0f; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsNotMoving() const { return Speed == 0.0f; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetYawSpeed() const { return YawSpeed; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSmoothedYawSpeed() const { return SmoothedYawSpeed; }
	
private:
	UPROPERTY()
	class ACharacter* OwnerCharacter; 
	
	UPROPERTY()
	class UCharacterMovementComponent* OwnerMovementComponent;
	
	float Speed; 
	float YawSpeed;
	float SmoothedYawSpeed; //平滑量
	UPROPERTY(EditAnywhere, Category="Animation")
	float YawSpeedSmoothLerp = 1.0f; //平滑量插值
	FRotator BodyPrevRotation; //上一帧身体旋转量
	
};
