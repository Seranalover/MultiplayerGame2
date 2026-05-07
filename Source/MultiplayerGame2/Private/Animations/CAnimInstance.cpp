// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/CAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/CAbilitySystemStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UCAnimInstance::NativeInitializeAnimation()
{
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComponent = OwnerCharacter->GetCharacterMovement();	
	}
	
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetAimStatTag()).AddUObject(this, &UCAnimInstance::OwnerAimTagChanged);
	}
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwnerCharacter)
	{
		FVector Velocity = OwnerCharacter->GetVelocity();
		Speed = Velocity.Length();
		FRotator BodyRotation = OwnerCharacter->GetActorRotation(); //当前身体旋转量
		FRotator BodyRotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRotation, BodyPrevRotation);
		BodyPrevRotation = BodyRotation;
		
		YawSpeed = BodyRotationDelta.Yaw / DeltaSeconds; //偏转速率
		float YawLerpSpeed = YawSpeedSmoothLerp;
		if (YawSpeed == 0) YawLerpSpeed = YawSpeedLerpToZeroSpeed; 
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaSeconds, YawLerpSpeed); //身体偏移平滑量
			
		FRotator ControlRotation = OwnerCharacter->GetBaseAimRotation(); 
		LookAtRotationOffset = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, BodyRotation); //瞄准偏移量
		
		FwdSpeed = Velocity.Dot(ControlRotation.Vector()); //向前速度 = 速度 点积 控制器面朝向量
		RightSpeed = -Velocity.Dot(ControlRotation.Vector().Cross(FVector::UpVector)); //-向右速度 = 速度 点积 （控制器面朝向量 叉乘 向上向量）
	}
	
	if (OwnerMovementComponent)
	{
		bIsJumping = OwnerMovementComponent->IsFalling();
	}
}

void UCAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
}

bool UCAnimInstance::ShouldDoFullBody() const
{
	return (GetSpeed() <= 0) && !GetIsAiming(); //禁止且不瞄准时
}

void UCAnimInstance::OwnerAimTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsAiming = NewCount != 0;
}
