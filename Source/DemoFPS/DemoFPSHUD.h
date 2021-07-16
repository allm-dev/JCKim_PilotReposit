// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DemoFPSHUD.generated.h"



UCLASS()
class ADemoFPSHUD : public AHUD
{
	GENERATED_BODY()

public:
	ADemoFPSHUD();

	UFUNCTION()
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void DrawHUD() override;

	UFUNCTION()
	void SetGameOver(bool b)
	{
		bGameOver = b;
	}

	UFUNCTION()
	bool GetGameOver() const { return bGameOver; }
	
private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	UPROPERTY()
	UFont* HudFont;

	UPROPERTY()
	bool bGameOver;

};


