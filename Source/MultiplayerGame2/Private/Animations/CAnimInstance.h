// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsJumping() const { return bIsJumping; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsOnGround() const { return !bIsJumping; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLookAtYawOffset() const { return LookAtRotationOffset.Yaw; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLookAtPitchOffset() const { return LookAtRotationOffset.Pitch; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetFwdSpeed() const { return FwdSpeed; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetRightSpeed() const { return RightSpeed; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	bool ShouldDoFullBody() const; //是否应该执行全身动作？
	
private:
	UPROPERTY()
	class ACharacter* OwnerCharacter; 
	
	UPROPERTY()
	class UCharacterMovementComponent* OwnerMovementComponent;
	
	void OwnerAimTagChanged(const FGameplayTag Tag, int32 NewCount);
	
	float Speed; 
	float YawSpeed;
	float SmoothedYawSpeed; //平滑量
	bool bIsJumping; //是否跳跃？
	
	UPROPERTY(EditAnywhere, Category="Animation")
	float YawSpeedSmoothLerp = 1.0f; //平滑量插值
	
	UPROPERTY(EditAnywhere, Category="Animation")
	float YawSpeedLerpToZeroSpeed = 30.f; //平滑量插值过渡到0的速度
	
	FRotator BodyPrevRotation; //上一帧身体旋转量
	FRotator LookAtRotationOffset; //瞄准偏移旋转
	bool bIsAiming;
	float FwdSpeed; //向前速度
	float RightSpeed; //向右速度
	
};
