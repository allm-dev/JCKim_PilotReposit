// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoFPSGameMode.h"
#include "DemoFPSHUD.h"
#include "DemoFPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADemoFPSGameMode::ADemoFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter.FirstPersonCharacter_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ADemoFPSHUD::StaticClass();
}
