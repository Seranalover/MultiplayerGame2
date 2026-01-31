// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CCharacter.h"

#include "Components/WidgetComponent.h"
#include "GAS/CAbilitySystemComponent.h"
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
}

//服务端初始化
void ACCharacter::ServerSideInit()
{
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
	CAbilitySystemComponent->ApplyInitialEffects(); //初始化属性值
}

//客户端初始化
void ACCharacter::ClientSideInit()
{
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
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

void ACCharacter::ConfigureOverheadWidget()
{
	if (!OverheadWidgetComponent) return;
	
	UOverheadStatsGauge* OverheadStatsGauge = Cast<UOverheadStatsGauge>(OverheadWidgetComponent->GetUserWidgetObject()); //获得控件
	if (OverheadStatsGauge)
		OverheadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent()); //设置控件属性值
}

