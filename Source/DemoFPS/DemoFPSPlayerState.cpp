// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoFPSPlayerState.h"

#include "DemoFPSCharacter.h"
#include "Net/UnrealNetwork.h"

ADemoFPSPlayerState::ADemoFPSPlayerState()
{
	KillScore = 0;
	MaxHP = 100;
	CurrentHP = 100;

	bReplicates = true;
}

void ADemoFPSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADemoFPSPlayerState, CurrentHP);
	DOREPLIFETIME(ADemoFPSPlayerState, MaxHP);
	DOREPLIFETIME(ADemoFPSPlayerState, KillScore);
}

void ADemoFPSPlayerState::AddKillScore(uint8 ScoreAmount)
{
	if (!HasAuthority())
	{
		return;
	}
	
	KillScore += ScoreAmount;
	UE_LOG(LogTemp, Warning, TEXT("Kill Score Up"));
}

void ADemoFPSPlayerState::AddCurrentHP(uint8 AddAmount)
{
	if (!HasAuthority())
	{
		return;
	}
	
	CurrentHP = FMath::Clamp<int32>(CurrentHP + AddAmount, 0, MaxHP);
}

void ADemoFPSPlayerState::LoseCurrentHP(uint8 LoseAmount)
{
	if (!HasAuthority())
	{
		return;
	}
	
	CurrentHP = FMath::Clamp<int32>(CurrentHP - LoseAmount, 0, 100);
	
	UE_LOG(LogTemp, Warning, TEXT("%s got %d damage"), *GetName(), LoseAmount)
	
	if (CurrentHP <= 0)
	{
		APlayerController* PC = GetOwner<APlayerController>();
		if (!IsValid(PC))
		{
			return;
		}
		
		ADemoFPSCharacter* Character = PC->GetPawn<ADemoFPSCharacter>();
		if (!IsValid(Character))
		{
			return;
		}
	
		Character->Destroy();
	}
}

