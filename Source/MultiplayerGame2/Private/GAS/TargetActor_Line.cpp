// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActor_Line.h"
#include "Components/SphereComponent.h"
#include "MultiplayerGame2/MultiplayerGame2.h"
#include "NiagaraComponent.h"

ATargetActor_Line::ATargetActor_Line()
{
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Component");
	SetRootComponent(RootComp);
	
	TargetEndDetectionSphere = CreateDefaultSubobject<USphereComponent>("Target End Detection Sphere");
	TargetEndDetectionSphere->SetupAttachment(GetRootComponent());
	TargetEndDetectionSphere->SetCollisionResponseToChannel(ECC_SpringArm, ECR_Ignore);
	
	LazerVFX = CreateDefaultSubobject<UNiagaraComponent>("Lazer VFX");
	LazerVFX->SetupAttachment(GetRootComponent());
	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true; //仅在server处理逻辑，在client显示模拟效果
	
	AvatarActor = nullptr;
}
