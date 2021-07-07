// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "MyCharacter.h"
#include "MyPlayerController.h"

AMyGameMode::AMyGameMode()
{
	//default pawn set-up
	
	//DefaultPawnClass = AMyPawn::StaticClass();
	DefaultPawnClass = AMyCharacter::StaticClass();
	
	//default PC set-up
	PlayerControllerClass = AMyPlayerController::StaticClass();
	
}

void AMyGameMode::PostLogin(APlayerController* NewPlayer)
{
	ABLOG(Warning, TEXT("PostLogin stage is about to start"));
	Super::PostLogin(NewPlayer);
	ABLOG(Warning, TEXT("PostLogin stage has been just completed"))
}

