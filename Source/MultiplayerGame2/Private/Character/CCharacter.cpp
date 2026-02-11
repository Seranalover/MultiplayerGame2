// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Widgets/OverheadStatsGauge.h"

// Sets default values
ACCharacter::ACCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //关闭骨骼碰撞
	// GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); //开启胶囊体碰撞
	
	CAbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>("CAbility System Component"); //创建GAS组件
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>("CAttribute Set"); //创建AS组件
	
	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Overhead Widget Component"); //创建头顶状态栏
	OverheadWidgetComponent->SetupAttachment(GetRootComponent()); //添加状态栏到根组件
	
	PerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("Perception Stimuli Source Component");
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

void ACCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCharacter, TeamId);
}

const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& ACCharacter::GetAbilities() const
{
	return CAbilitySystemComponent->GetAbilities();
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
	MeshRelativeTransform = GetMesh()->GetRelativeTransform();
	ConfigureOverheadWidget();
	BindGASChangeDelegates();
	
	PerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass()); //注册感知功能
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

void ACCharacter::Server_SendGameplayEventToSelf_Implementation(const FGameplayTag& EventTag,
	const FGameplayEventData& EventData)
{	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
}

bool ACCharacter::Server_SendGameplayEventToSelf_Validate(const FGameplayTag& EventTag,
	const FGameplayEventData& EventData)
{
	//验证失败返回false时，会立即断开验证失败的客户端连接
	return true;
}

void ACCharacter::BindGASChangeDelegates()
{
	if (CAbilitySystemComponent)
	{
		//监听Dead Tag，绑定DeathTagUpdated()
		CAbilitySystemComponent->RegisterGameplayTagEvent(
			UCAbilitySystemStatics::GetDeadStatTag()).AddUObject(this, &ACCharacter::DeathTagUpdated);
		
		CAbilitySystemComponent->RegisterGameplayTagEvent(
			UCAbilitySystemStatics::GetStunStatTag()).AddUObject(this, &ACCharacter::StunTagUpdated);
		
		CAbilitySystemComponent->RegisterGameplayTagEvent(
			UCAbilitySystemStatics::GetAimStatTag()).AddUObject(this, &ACCharacter::AimTagUpdated);
	}
}

void ACCharacter::StunTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (IsDead()) return;
	if (NewCount != 0)
	{
		OnStun();
		PlayAnimMontage(StunAnimMontage);
	}
	else
	{
		OnRecoverFromStun();
		StopAnimMontage(StunAnimMontage);
	}
}

void ACCharacter::AimTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	SetIsAiming(NewCount != 0);
}

void ACCharacter::SetIsAiming(bool bIsAiming)
{
	bUseControllerRotationYaw = bIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bIsAiming;
}

void ACCharacter::OnStun()
{
}

void ACCharacter::OnRecoverFromStun()
{
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
	// 关键检查：确保不是CDO
	if (HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		UE_LOG(LogTemp, Fatal, TEXT("FATAL: Modifying CDO! Actor: %s"), *GetName());
		return;
	}
	
	// UE_LOG(LogTemp, Warning, TEXT("MultiplayerGame2 Error: StartDeathSequence"));
	OnDead(); //因当前类可能是AI，所以在子类关闭控制器输入
	if (CAbilitySystemComponent) CAbilitySystemComponent->CancelAbilities(); //取消所有技能，包括正在执行的技能
	PlayDeathAnimation(); //播放死亡蒙太奇
	SetStatsGaugeEnabled(false); //关闭血条显示
	// GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None); //禁用移动
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //禁用碰撞
	SetAIPerceptionStimuliSourceEnabled(false); //关闭感知刺激源
}

void ACCharacter::Respawn()
{
	// UE_LOG(LogTemp, Warning, TEXT("MultiplayerGame2 Error: Respawn"));
	OnRespawn(); //在子类开启控制器输入
	SetRagdollEnabled(false); //关闭布偶效果
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); //恢复碰撞
	// GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking); //恢复移动
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f); //关闭所有动画
	SetStatsGaugeEnabled(true); //恢复血条显示
	SetAIPerceptionStimuliSourceEnabled(true); //开启感知刺激源
	
	if (HasAuthority() && GetController())
	{
		TWeakObjectPtr<AActor> StartSpot = GetController()->StartSpot;
		if (StartSpot.IsValid())
			SetActorTransform(StartSpot->GetActorTransform());
	}
	
	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->ApplyFullStatEffect(); //回复所有状态
	}
}

void ACCharacter::PlayDeathAnimation()
{
	if (DeathAnimMontage)
	{
		float MontageDuration = PlayAnimMontage(DeathAnimMontage);
		GetWorldTimerManager().SetTimer(DeathMontageTimerHandle, this,
			&ACCharacter::DeathMontageFinished, MontageDuration + DeathMontageFinishTimeShift);
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
{ //override in child class
}

void ACCharacter::OnRespawn()
{ //override in child class
}

void ACCharacter::DeathMontageFinished()
{
	if (IsDead())
		SetRagdollEnabled(true);
}

void ACCharacter::SetRagdollEnabled(bool bIsEnable)
{
	if (bIsEnable)
	{
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); //分离骨骼
		GetMesh()->SetSimulatePhysics(true); //开启模拟物理
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly); //开启骨骼碰撞
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(MeshRelativeTransform); //设置骨骼相对变换
	}
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
	// APawn* LocalPlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn(); //获取local player pawn，AI用该方法会导致空指针
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0); //获取local player pawn
	if (LocalPlayerPawn)
	{
		float DistanceSquared = FVector::DistSquared(LocalPlayerPawn->GetActorLocation(), GetActorLocation()); //距离平方
		OverheadWidgetComponent->SetHiddenInGame(DistanceSquared > OverheadWidgetVisibilityRangeSquared); //距离过远隐藏头顶状态栏
	}
}

bool ACCharacter::IsDead() const
{
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(UCAbilitySystemStatics::GetDeadStatTag()); //是否存活？

}

void ACCharacter::RespawnImmediately()
{
	if (HasAuthority())
		GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(UCAbilitySystemStatics::GetDeadStatTag())); //移除dead tag
}

void ACCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

FGenericTeamId ACCharacter::GetGenericTeamId() const
{
	return TeamId;
}

void ACCharacter::SetAIPerceptionStimuliSourceEnabled(bool bIsEnable)
{
	if (!PerceptionStimuliSourceComponent) return;
	if (bIsEnable)
		PerceptionStimuliSourceComponent->RegisterWithPerceptionSystem(); //刺激源注册到感知系统
	else
		PerceptionStimuliSourceComponent->UnregisterFromPerceptionSystem(); //取消注册
}

void ACCharacter::OnRep_TeamID()
{
	//override in child class
}
