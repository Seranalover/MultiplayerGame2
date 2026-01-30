// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/CAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCAnimInstance::NativeInitializeAnimation()
{
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComponent = OwnerCharacter->GetCharacterMovement();	
	}
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwnerCharacter)
	{
		Speed = OwnerCharacter->GetVelocity().Length();
		FRotator BodyRotation = OwnerCharacter->GetActorRotation(); //当前身体旋转量
		FRotator BodyRotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRotation, BodyPrevRotation);
		BodyPrevRotation = BodyRotation;
		
		YawSpeed = BodyRotationDelta.Yaw / DeltaSeconds; //偏转速率
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaSeconds, YawSpeedSmoothLerp);
		
	}
	
	if (OwnerMovementComponent)
	{
		bIsJumping = OwnerMovementComponent->IsFalling();
	}
}

void UCAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
}
