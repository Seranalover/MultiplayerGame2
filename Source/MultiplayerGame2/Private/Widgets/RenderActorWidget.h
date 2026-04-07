// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RenderActorWidget.generated.h"

class ARenderActor;
/**
 * 特写渲染控件类
 */
UCLASS(Abstract)
class URenderActorWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	class UImage* DisplayImage;
	
	UPROPERTY(meta=(BindWidget))
	class USizeBox* RenderSizeBox;
	
	UPROPERTY(EditDefaultsOnly, Category="Render Actor")
	FName DisplayImageRenderTargetParamName = "Render Target";
	
	UPROPERTY(EditDefaultsOnly, Category="Render Actor")
	FVector2D RenderSize;
	
	UPROPERTY(EditDefaultsOnly, Category="Render Actor")
	int FrameRate = 24;
	
	float RenderTickInterval; //渲染间隔
	FTimerHandle RenderTimerHandle; //渲染计时器
	
	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget;
	
	// virtual void SpawnRenderActor() = 0; //改写法为纯C++语法，UE5中不支持这样定义纯虚函数
	virtual void SpawnRenderActor() PURE_VIRTUAL(URenderActorWidget::SpawnRenderActor, /*不带参数*/);
	virtual ARenderActor* GetRenderActor() const PURE_VIRTUAL(URenderActorWidget::GetRenderActor, return nullptr; );
	
	void ConfigureRenderActor();
	void BeginRenderCapture();
	void UpdateRender();
	void EndRenderCapture();
};
