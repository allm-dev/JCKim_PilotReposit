// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoFPSGameMode.h"
#include "DemoFPSCharacter.h"
#include "DemoFPSGameState.h"
#include "DemoFPSHUD.h"
#include "DemoFPSPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ADemoFPSGameMode::ADemoFPSGameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick =true;
		
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter.FirstPersonCharacter_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = ADemoFPSHUD::StaticClass();
	GameStateClass = ADemoFPSGameState::StaticClass();
	PlayerStateClass = ADemoFPSPlayerState::StaticClass();

	bReplicates = true;
	
}


