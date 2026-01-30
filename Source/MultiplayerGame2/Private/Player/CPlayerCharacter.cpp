// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

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
	}
}

//旋转视角方法实现
void ACPlayerCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputVector2D = InputActionValue.Get<FVector2D>(); //获得2d输入
	AddControllerPitchInput(-InputVector2D.Y);
	AddControllerYawInput(InputVector2D.X);
}
