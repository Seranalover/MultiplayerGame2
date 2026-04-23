// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RenderActorTargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URenderActorTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 角色特写渲染参数接口类
 */
class IRenderActorTargetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FVector GetCaptureLocalPosition() const = 0; //捕捉相对位置
	virtual FRotator GetCaptureLocalRotation() const = 0; //捕捉相对旋转
};
