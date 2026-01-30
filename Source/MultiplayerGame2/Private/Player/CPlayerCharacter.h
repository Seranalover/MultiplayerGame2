// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Character/CCharacter.h"
#include "CPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ACPlayerCharacter : public ACCharacter
{
	GENERATED_BODY()
	
public:
	ACPlayerCharacter();
	virtual void PawnClientRestart() override; //客户端生成pawn时调用
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
private:
	UPROPERTY(VisibleDefaultsOnly, Category="View")
	class USpringArmComponent* CameraBoom; //弹簧臂组件
	
	UPROPERTY(VisibleDefaultsOnly, Category="View")
	class UCameraComponent* ViewCamera; //相机组件
	
	/*****************************************************************/
	/*                             Input                             */
	/*****************************************************************/
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputMappingContext* GameplayInputMappingContext; //IM上下文
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* JumpInputAction; //跳跃
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* LookInputAction; //视角旋转
	
	void HandleLookInput(const FInputActionValue& InputActionValue); //视角旋转实现
};
