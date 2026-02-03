// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CAttributeSet.h"
#include "Widgets/OverheadStatsGauge.h"

// Sets default values
ACCharacter::ACCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //关闭自身碰撞
	
	CAbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>("CAbility System Component"); //创建GAS组件
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>("CAttribute Set"); //创建AS组件
	
	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Overhead Widget Component"); //创建头顶状态栏
	OverheadWidgetComponent->SetupAttachment(GetRootComponent()); //添加状态栏到根组件
	
	BindGASChangeDelegates();
}

//服务端初始化
void ACCharacter::ServerSideInit()
{
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
	CAbilitySystemComponent->ApplyInitialEffects(); //初始化属性值
	CAbilitySystemComponent->GiveInitialAbilities(); //初始化能力，不用在客户端调用，根据文档，它会自动同步到客户端
}

//客户端初始化
void ACCharacter::ClientSideInit()
{
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
}

bool ACCharacter::IsLocallyControlledByPlayer() const
{
	//控制器不为空且为本地玩家控制器？
	return GetController() && GetController()->IsLocalPlayerController();
}

//only called on the Server
void ACCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController()) //如果是AI控制器
	{
		ServerSideInit();
	}
}

// Called when the game starts or when spawned
void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverheadWidget();
}

// Called every frame
void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{ 
	return CAbilitySystemComponent;
}

void ACCharacter::BindGASChangeDelegates()
{
	if (CAbilitySystemComponent)
	{
		//监听Dead Tag，绑定DeathTagUpdated()
		CAbilitySystemComponent->RegisterGameplayTagEvent(
			UCAbilitySystemStatics::GetDeadStatTag()).AddUObject(this, &ACCharacter::DeathTagUpdated);
	}
}

void ACCharacter::DeathTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0)
	{
		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
}

void ACCharacter::StartDeathSequence()
{
	UE_LOG(LogTemp, Warning, TEXT("MultiplayerGame2 Error: StartDeathSequence"));
	OnDead(); //因当前类可能是AI，所以在子类关闭控制器输入
	PlayDeathAnimation(); //播放死亡蒙太奇
	// SetStatsGaugeEnabled(false); //关闭血条显示
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None); //禁用移动
	// GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //禁用碰撞
}

void ACCharacter::Respawn()
{
	UE_LOG(LogTemp, Warning, TEXT("MultiplayerGame2 Error: Respawn"));
	OnRespawn();
}

void ACCharacter::PlayDeathAnimation()
{
	if (DeathAnimMontage)
	{
		PlayAnimMontage(DeathAnimMontage);
	}
		
}

void ACCharacter::SetStatsGaugeEnabled(bool bIsEnable)
{
	GetWorldTimerManager().ClearTimer(OverheadWidgetVisibilityTimerHandle); //清除计时器
	if (bIsEnable)
		ConfigureOverheadWidget();
	else
		OverheadWidgetComponent->SetHiddenInGame(true);
}

void ACCharacter::OnDead()
{
}

void ACCharacter::OnRespawn()
{
}

void ACCharacter::ConfigureOverheadWidget()
{
	if (!OverheadWidgetComponent) return;
	
	if (IsLocallyControlledByPlayer())
	{
		//隐藏本地玩家头顶状态栏
		OverheadWidgetComponent->SetHiddenInGame(true);
		return;
	}
	
	UOverheadStatsGauge* OverheadStatsGauge = Cast<UOverheadStatsGauge>(OverheadWidgetComponent->GetUserWidgetObject()); //获得控件
	if (OverheadStatsGauge)
	{
		OverheadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent()); //设置控件属性值
		OverheadWidgetComponent->SetHiddenInGame(false); 
		
		GetWorldTimerManager().ClearTimer(OverheadWidgetVisibilityTimerHandle); //清除计时器
		GetWorldTimerManager().SetTimer(OverheadWidgetVisibilityTimerHandle, this, &ACCharacter::UpdateOverheadWidgetVisibility, 
			OverheadWidgetVisibilityCheckGap, true); //设置计时器
	}
}

void ACCharacter::UpdateOverheadWidgetVisibility()
{
	APawn* LocalPlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn(); //获取local player pawn
	if (LocalPlayerPawn)
	{
		float DistanceSquared = FVector::DistSquared(LocalPlayerPawn->GetActorLocation(), GetActorLocation()); //距离平方
		OverheadWidgetComponent->SetHiddenInGame(DistanceSquared > OverheadWidgetVisibilityRangeSquared); //距离过远隐藏头顶状态栏
	}
}

