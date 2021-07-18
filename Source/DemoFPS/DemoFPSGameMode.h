// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DemoFPSGameMode.generated.h"

DECLARE_DELEGATE_OneParam(OnGameOverDelegate, bool);

UCLASS(minimalapi)
class ADemoFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	float RunTime;

	UPROPERTY(EditAnywhere, Category=PlayTime, Meta=(AllowPrivateAccess=true))
	float MaxPlayTime;

public:
	ADemoFPSGameMode();

	float GetRunTime() const {return RunTime;};

	float GetRemainingTime() const {return MaxPlayTime - RunTime;}

	void SetGamePaused (bool bIsGameOver);

	UFUNCTION()
	virtual void Tick(float DeltaSeconds) override;

	OnGameOverDelegate OnGameOver;
};



