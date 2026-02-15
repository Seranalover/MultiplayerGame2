// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CHeroAttributeSet.h"
#include "MultiplayerGame2/MultiplayerGame2.h"

ACPlayerCharacter::ACPlayerCharacter()
{
	//添加弹簧臂组件
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	CameraBoom->SetupAttachment(GetRootComponent()); 
	CameraBoom->bUsePawnControlRotation = true; //弹簧臂使用pawn的控制旋转
	CameraBoom->ProbeChannel = ECC_SpringArm; //设置弹簧臂碰撞检测渠道
	
	//添加相机组件
	ViewCamera = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	ViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	
	bUseControllerRotationYaw = false; //禁用控制器yaw旋转
	GetCharacterMovement()->bOrientRotationToMovement = true; //角色面朝移动方向
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); //旋转速率
	
	HeroAttributeSet = CreateDefaultSubobject<UCHeroAttributeSet>("Hero Attribute Set");
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
	if (AbilityInputID == ECAbilityInputID::BasicAttack) //添加basic attack input tag事件到actor
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, 
			UCAbilitySystemStatics::GetBasicAttackInputPressedTag(), FGameplayEventData()); //client only
		Server_SendGameplayEventToSelf(UCAbilitySystemStatics::GetBasicAttackInputPressedTag(), FGameplayEventData()); //copy to server
	}
}

void ACPlayerCharacter::SetInputEnabled(bool bEnabled)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!PlayerController) return;
	if (bEnabled)
		EnableInput(PlayerController);
	else
		DisableInput(PlayerController);
	
}

void ACPlayerCharacter::OnDead()
{
	SetInputEnabled(false); //禁用输入
}

void ACPlayerCharacter::OnRespawn()
{
	SetInputEnabled(true); 
}

void ACPlayerCharacter::OnStun()
{
	SetInputEnabled(false); //禁用输入
}

void ACPlayerCharacter::OnRecoverFromStun()
{
	if (IsDead()) return;
	SetInputEnabled(true);
}

void ACPlayerCharacter::OnAimStateChanged(bool bIsAiming)
{
	LerpCameraToLocalOffsetLocation(bIsAiming ? CameraAimLocalOffset : FVector::ZeroVector);
}

void ACPlayerCharacter::LerpCameraToLocalOffsetLocation(const FVector& Goal)
{
	GetWorldTimerManager().ClearTimer(CameraLerpTimerHandle);
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, & ACPlayerCharacter::TickCameraLocalOffsetLerp, Goal)); //在下一次tick时创建插值计时器
}

void ACPlayerCharacter::TickCameraLocalOffsetLerp(const FVector Goal)
{
	FVector CurrentLocalOffset = ViewCamera->GetRelativeLocation();
	if (FVector::Dist(CurrentLocalOffset, Goal) < 1.f)
	{
		ViewCamera->SetRelativeLocation(Goal);
		return;
	}
	
	float LerpAlpha = FMath::Clamp(GetWorld()->GetDeltaSeconds() * CameraLerpSpeed, 0.f, 1.f); //计算插值
	FVector NewLocalOffset = FMath::Lerp(CurrentLocalOffset, Goal, LerpAlpha); //计算下一帧相机位置
	ViewCamera->SetRelativeLocation(NewLocalOffset); //移动相机
	
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, & ACPlayerCharacter::TickCameraLocalOffsetLerp, Goal)); //在下一次tick时创建插值计时器
}
