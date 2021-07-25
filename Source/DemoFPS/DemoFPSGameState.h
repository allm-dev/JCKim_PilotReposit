// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <mfidl.h>

#include "GameFramework/GameState.h"
#include "DemoFPSGameState.generated.h"

/**
 * 
 */
UCLASS()
class DEMOFPS_API ADemoFPSGameState : public AGameState
{
	GENERATED_BODY()

public:
	
	ADemoFPSGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaSeconds) override;
	
	float GetRunTime() const
	{
		return RunTime;
	}

    float GetRemainingTime() const
    {
    	return FMath::Clamp(MaxPlayTime - RunTime,0.00f, MaxPlayTime);
    }

	UPROPERTY()
	float RunTime;

	UPROPERTY(Replicated)
	float MaxPlayTime;

	UPROPERTY(Replicated)
	bool bIsGameOver;

	UFUNCTION()
	bool GetIsGameOver() const
	{
		return bIsGameOver;
	}
	
	UFUNCTION()
	void SetIsGameOver(bool IsGameOver)
	{
		bIsGameOver = IsGameOver;
	}

	UFUNCTION()
	void ShowResultHUD();

	UFUNCTION()
	void PauseGame();
};
