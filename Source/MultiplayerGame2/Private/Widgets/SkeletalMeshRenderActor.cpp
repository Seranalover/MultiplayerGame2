// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SkeletalMeshRenderActor.h"

ASkeletalMeshRenderActor::ASkeletalMeshRenderActor()
{
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh Component");
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetLightingChannels(false, true, false); //将第二通道作为网格体的专属照明通道，确保第一通道的环境光不会影响头部渲染器
}

void ASkeletalMeshRenderActor::ConfigureSkeletalMesh(USkeletalMesh* MeshAsset, TSubclassOf<UAnimInstance> AnimBlueprint)
{
	MeshComponent->SetSkeletalMeshAsset(MeshAsset);
	MeshComponent->SetAnimInstanceClass(AnimBlueprint);
}

void ASkeletalMeshRenderActor::BeginPlay()
{
	Super::BeginPlay();
	MeshComponent->SetVisibleInSceneCaptureOnly(true); //将网格体设置为仅环境捕捉可见
}
