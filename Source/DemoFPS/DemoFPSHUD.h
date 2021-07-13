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

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

