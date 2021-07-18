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

void ADemoFPSGameMode::SetGamePaused(bool bIsGameOver)
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
		
	APlayerController* PlayerCt = World->GetFirstPlayerController();
	if (IsValid(PlayerCt))
	{
		OnGameOver.Execute(true);
		PlayerCt->SetPause(bIsGameOver);
	}
}

void ADemoFPSGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//UGamePlayStatics::GetTimeSeconds의 반환값은 게임이 재시작(OpenLevel)될 때 초기화되지 않는다.
	if (RunTime >= MaxPlayTime)
	{
		SetGamePaused(true);
	}
	
	else
	{
		RunTime += DeltaSeconds;
	}
}

