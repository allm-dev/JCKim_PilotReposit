// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DemoFPSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DEMOFPS_API ADemoFPSPlayerState : public APlayerState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category=KillScore, meta = (AllowPrivateAccess), Replicated)
	uint8 KillScore;

	UPROPERTY(VisibleAnywhere, Category=HealthPoint, meta = (AllowPrivateAccess), Replicated)
	uint8 CurrentHP;

	UPROPERTY(VisibleAnywhere, Category=HealthPoint, meta = (AllowPrivateAccess), Replicated)
	uint8 MaxHP;

public:
	ADemoFPSPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	uint8 GetKillScore() const
	{
		return KillScore;
	}

	UFUNCTION(BlueprintCallable)
	uint8 GetCurrentHP() const
	{
		return CurrentHP;
	}

	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
	void AddKillScore(uint8 ScoreAmount);

	UFUNCTION()
	void AddCurrentHP(uint8 AddAmount);

	UFUNCTION()
	void LoseCurrentHP(uint8 LoseAmount);


};
