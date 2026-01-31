// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACPlayerCharacter::ACPlayerCharacter()
{
	//添加弹簧臂组件
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	CameraBoom->SetupAttachment(GetRootComponent()); 
	CameraBoom->bUsePawnControlRotation = true; //弹簧臂使用pawn的控制旋转
	
	//添加相机组件
	ViewCamera = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	ViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	
	bUseControllerRotationYaw = false; //禁用控制器yaw旋转
	GetCharacterMovement()->bOrientRotationToMovement = true; //角色面朝移动方向
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); //旋转速率
}

void ACPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	APlayerController* OwningPlayerController = GetController<APlayerController>(); //获得玩家控制器
	if (OwningPlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem = 
			OwningPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(); //本地玩家的增强输入系统
		if (InputLocalPlayerSubsystem)
		{
			InputLocalPlayerSubsystem->RemoveMappingContext(GameplayInputMappingContext);
			InputLocalPlayerSubsystem->AddMappingContext(GameplayInputMappingContext, 0); //为本地玩家控制器配置IM
		}
	}
}

void ACPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::Jump); //绑定跳跃方法到跳跃IA，跳跃方法UE已实现
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleLookInput); //绑定转动视角方法到视角IA
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleMoveInput); //移动IA
		
		for (const TPair<ECAbilityInputID, UInputAction*>& InputActionPair : GameplayAbilityInputActions)
		{
			EnhancedInputComponent->BindAction(InputActionPair.Value, ETriggerEvent::Triggered, 
				this, &ACPlayerCharacter::HandleAbilityInput, InputActionPair.Key); //绑定所有技能到IA

		}
	}
}

//旋转视角方法实现
void ACPlayerCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputVector2D = InputActionValue.Get<FVector2D>(); //获得2d输入
	AddControllerPitchInput(-InputVector2D.Y);
	AddControllerYawInput(InputVector2D.X);
}

void ACPlayerCharacter::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputVector2D = InputActionValue.Get<FVector2D>(); //获得2d输入
	InputVector2D.Normalize();
	
	AddMovementInput(GetMoveForwardDirection()*InputVector2D.Y + GetLookRightDirection()*InputVector2D.X);
}

FVector ACPlayerCharacter::GetLookRightDirection() const
{
	return ViewCamera->GetRightVector();
}

FVector ACPlayerCharacter::GetLookForwardDirection() const
{
	return ViewCamera->GetForwardVector();
}

FVector ACPlayerCharacter::GetMoveForwardDirection() const
{
	//通过向右视角方向，和向上向量，计算叉乘，得到向前移动向量，开销大，但是过渡平滑
	return FVector::CrossProduct(GetLookRightDirection(), FVector::UpVector);
}

void ACPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID AbilityInputID)
{
	bool bPressed = InputActionValue.Get<bool>(); //是否按下？
	if (bPressed)
	{
		GetAbilitySystemComponent()->AbilityLocalInputPressed((int32)AbilityInputID);
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased((int32)AbilityInputID);
	}
}
