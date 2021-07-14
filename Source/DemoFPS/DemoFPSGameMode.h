// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DemoFPSGameMode.generated.h"

UCLASS(minimalapi)
class ADemoFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	float RunTime;

	UPROPERTY()
	float MaxPlayTime;

	UPROPERTY()
	bool bGameOver;

public:
	ADemoFPSGameMode();

	float GetRunTime() const {return RunTime;};

	float GetRemainingTime() const {return MaxPlayTime - RunTime;}

	UFUNCTION()
	virtual void Tick(float DeltaSeconds) override;
};



