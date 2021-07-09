// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterStatComponent.h"
#include "MyGameInstance.h"

// Sets default values for this component's properties
UMyCharacterStatComponent::UMyCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent =true;

	Level = 1;
}


// Called when the game starts
void UMyCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UMyCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(Level);
}

void UMyCharacterStatComponent::SetNewLevel(int32 NewLevel)
{
	auto MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	MYCHECK(nullptr!= MyGameInstance)
	CurrentStatData = MyGameInstance->GetMyCharacterData(NewLevel);
	if(CurrentStatData != nullptr)
	{
		Level = NewLevel;
		SetHP(CurrentStatData->MaxHP);
	}
	else
	{
		ABLOG(Error, TEXT("Level (%d) Data doesn't exist"), NewLevel);
	}
}

void UMyCharacterStatComponent::SetDamage(float NewDamage)
{
	MYCHECK(CurrentStatData !=nullptr);
	SetHP(FMath::Clamp<float>(CurrentHP-NewDamage, 0.0f, CurrentStatData->MaxHP));
}

void UMyCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();
	//float epsilon
	if(CurrentHP< KINDA_SMALL_NUMBER)
	{
		CurrentHP =0.0f;
		OnHPIsZero.Broadcast();
	}
}

float UMyCharacterStatComponent::GetHPRatio()
{
	MYCHECK(CurrentStatData !=nullptr, 0.0f);

	return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP/CurrentStatData->MaxHP);
}

float UMyCharacterStatComponent::GetAttack()
{
	MYCHECK(CurrentStatData != nullptr, 0.0f);
	return CurrentStatData->Attack;
}


