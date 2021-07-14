// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoFPSGameMode.h"
#include "DemoFPSHUD.h"
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

void ADemoFPSGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	RunTime += DeltaSeconds;
}
