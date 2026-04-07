// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RenderActorWidget.h"

#include "RenderActor.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/TextureRenderTarget2D.h"

void URenderActorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	RenderSizeBox->SetWidthOverride(RenderSize.X);
	RenderSizeBox->SetHeightOverride(RenderSize.Y);
}

void URenderActorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SpawnRenderActor();
	ConfigureRenderActor();
	BeginRenderCapture();
}

void URenderActorWidget::BeginDestroy()
{
	EndRenderCapture();
	Super::BeginDestroy();
}

void URenderActorWidget::ConfigureRenderActor()
{
	if (!GetRenderActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Render Actor, nothing will be rendered"));
		return;
	}
	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat((int)RenderSize.X, (int)RenderSize.Y);
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8_SRGB;
	GetRenderActor()->SetRenderTarget(RenderTarget);
	
	UMaterialInstanceDynamic* DisplayImageDynamicMaterial = DisplayImage->GetDynamicMaterial();
	if (DisplayImageDynamicMaterial)
		DisplayImageDynamicMaterial->SetTextureParameterValue(DisplayImageRenderTargetParamName, RenderTarget);
}

void URenderActorWidget::BeginRenderCapture()
{
	RenderTickInterval = 1.0f / (float)FrameRate;
	UWorld* World = GetWorld();
	if (World)
		World->GetTimerManager().SetTimer(RenderTimerHandle, this, &URenderActorWidget::UpdateRender, RenderTickInterval, true);
}

void URenderActorWidget::UpdateRender()
{
	if (GetRenderActor())
		GetRenderActor()->UpdateRenderTarget();
}

void URenderActorWidget::EndRenderCapture()
{
	UWorld* World = GetWorld();
	if (World)
		World->GetTimerManager().ClearTimer(RenderTimerHandle);
}
