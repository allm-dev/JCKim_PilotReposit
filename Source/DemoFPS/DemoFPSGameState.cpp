// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoFPSGameState.h"
#include "DemoFPSHUD.h"
#include "DemoFPSPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


ADemoFPSGameState::ADemoFPSGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RunTime = 0.0f;
	MaxPlayTime = 60.0f;
}

void ADemoFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADemoFPSGameState, MaxPlayTime);
	DOREPLIFETIME(ADemoFPSGameState, bIsGameOver);
}

void ADemoFPSGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	
	if (MaxPlayTime < RunTime && RunTime < MaxPlayTime + 0.2f)
	{
		bIsGameOver = true;
		ShowResultHUD();
	}

	if (RunTime >= MaxPlayTime + 0.2f)
	{
		PauseGame();
	}
	else
	{
		RunTime += DeltaSeconds;
	}
}


void ADemoFPSGameState::ShowResultHUD()
{
	if (HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!IsValid(PC))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get User Playercontroller"));
		return;
	}

	ADemoFPSHUD* Hud = PC->GetHUD<ADemoFPSHUD>();
	if (!IsValid(Hud))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast failed to get User HUD"));
		return;
	}

	Hud->DrawHUD();
}

void ADemoFPSGameState::PauseGame()
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	APlayerController* thisPC = UGameplayStatics::GetPlayerController(World, 0);
	if(IsValid(thisPC))
	{
		thisPC->ServerPause();
	}
}

