// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CGameplayAbility.h"

class UAnimInstance* UCGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMeshComponent = GetOwningComponentFromActorInfo(); //获得骨骼组件
	if (OwnerSkeletalMeshComponent) return OwnerSkeletalMeshComponent->GetAnimInstance(); //返回动画实例
	return nullptr;
}
