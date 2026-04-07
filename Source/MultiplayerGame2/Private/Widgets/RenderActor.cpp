// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RenderActor.h"

#include "Components/SceneCaptureComponent2D.h"

// Sets default values
ARenderActor::ARenderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Comp");
	SetRootComponent(RootComp);
	
	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture Component");
	CaptureComponent->SetupAttachment(RootComp);
	
	CaptureComponent->bCaptureEveryFrame = false; //关闭每帧渲染，开销很大，因为实际上是在渲染另一个viewport
	CaptureComponent->FOVAngle = 30.f; //避免头部拉伸
}

void ARenderActor::SetRenderTarget(class UTextureRenderTarget2D* RenderTarget)
{
	CaptureComponent->TextureTarget = RenderTarget;
}

void ARenderActor::UpdateRenderTarget()
{
	if (CaptureComponent)
		CaptureComponent->CaptureScene();
}

// Called when the game starts or when spawned
void ARenderActor::BeginPlay()
{
	Super::BeginPlay();
	CaptureComponent->ShowOnlyActorComponents(this); //使Capture Comp只渲染当前actor，忽视环境中的其他因素
	SetActorLocation(FVector{0.f, 100000.f, 0.f}); //将渲染对象移到地图外，避免游戏中的元素产生任何交互
}

