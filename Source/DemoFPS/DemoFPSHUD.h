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
	virtual void DrawHUD() override;

private:
	
	UPROPERTY()
	class UTexture2D* CrosshairTex;

	UPROPERTY()
	UFont* HudFont;

};


