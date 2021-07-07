// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DemoProject.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DEMOPROJECT_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()

	AMyGameMode();

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;	
};
