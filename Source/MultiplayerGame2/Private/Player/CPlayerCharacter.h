// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Character/CCharacter.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "CPlayerCharacter.generated.h"

/**
 * 玩家基类
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
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* MoveInputAction; //移动
	
	void HandleLookInput(const FInputActionValue& InputActionValue); //视角旋转实现
	void HandleMoveInput(const FInputActionValue& InputActionValue); //移动功能实现
	
	FVector GetLookRightDirection() const; //向前视角方向，包含向前上下看
	FVector GetLookForwardDirection() const; //向右视角方向
	FVector GetMoveForwardDirection() const; //向前移动方向
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TMap<ECAbilityInputID, class UInputAction*> GameplayAbilityInputActions; //所有技能id与IA的map集合
	void HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID AbilityInputID); //施放技能实现
	
	/*****************************************************************/
	/*                        Death and Respawn                      */
	/*****************************************************************/
	virtual void OnDead() override;
	virtual void OnRespawn() override;
};
