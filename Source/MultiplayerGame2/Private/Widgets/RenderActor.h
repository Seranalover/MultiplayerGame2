// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RenderActor.generated.h"

/**
 * 特写渲染基类
 */
UCLASS()
class ARenderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARenderActor();
	void SetRenderTarget(class UTextureRenderTarget2D* RenderTarget); //设置渲染目标
	void UpdateRenderTarget(); //更新渲染
	FORCEINLINE class USceneCaptureComponent2D* GetCaptureComponent() const { return CaptureComponent; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category="Render Actor")
	class USceneComponent* RootComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Render Actor")
	class USceneCaptureComponent2D* CaptureComponent;

};
