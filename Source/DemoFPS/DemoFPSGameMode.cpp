// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoFPSGameMode.h"
#include "DemoFPSHUD.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ADemoFPSGameMode::ADemoFPSGameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick =true;
		
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter.FirstPersonCharacter_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ADemoFPSHUD::StaticClass();

	RunTime =0.0f;
	MaxPlayTime = 60.0f;
}

void ADemoFPSGameMode::SetGamePaused(bool b)
{
	APlayerController* PlayerCt = GetWorld()->GetFirstPlayerController();
	if (PlayerCt != nullptr)
	{
		OnGameOver.Execute(true);
		PlayerCt->SetPause(b);
	}
}

void ADemoFPSGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(RunTime >= MaxPlayTime)
	{
		SetGamePaused(true);
	}
	else
	{
		RunTime += DeltaSeconds;
	}
}

