// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoFPSHUD.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"
#include "DemoFPSCharacter.h"
#include "DemoFPSGameMode.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"

ADemoFPSHUD::ADemoFPSHUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	if(CrosshairTexObj.Succeeded()) CrosshairTex = CrosshairTexObj.Object;

	static ConstructorHelpers::FObjectFinder<UFont> NewFont(TEXT("/Game/Font/NewFont.NewFont"));
	if(NewFont.Succeeded()) HudFont = NewFont.Object;

}


void ADemoFPSHUD::DrawHUD()
{
	Super::DrawHUD();

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	ADemoFPSCharacter* MyCharacter = Cast<ADemoFPSCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	ADemoFPSGameMode* MyGameMode = Cast<ADemoFPSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if(MyCharacter != nullptr && MyGameMode != nullptr)
	{
		const FString KillScore = FString::Printf(TEXT("Kill Score : %d"), MyCharacter->GetKillScore());
		DrawText(KillScore, FColor::Yellow, Center.X + 100, Center.Y-Canvas->ClipY*0.45f, HudFont);

		const FString RunTimeCheck = FString::Printf(TEXT("Remaining time : %.2f"), MyGameMode->GetRemainingTime());
		DrawText(RunTimeCheck, FColor::Red, Center.X-200, Center.Y-Canvas->ClipY*0.45f, HudFont);

		if(MyCharacter->GetCurrentWeapon() != nullptr)
		{
			const FString AmmunitionStat = FString::Printf(TEXT("Ammo : %d / %d"), MyCharacter->GetCurrentWeapon()->GetCurrentAmmo(), MyCharacter->GetAmmoCount());
			DrawText(AmmunitionStat, FColor::White,Canvas->ClipX-200, Canvas->ClipY-75, HudFont);
		}
		
		const FString GrenadeStat = FString::Printf(TEXT("Grenade : %d"), MyCharacter->GetGrenadeCount());
		DrawText(GrenadeStat, FColor::Silver,Canvas->ClipX-200, Canvas->ClipY-35, HudFont);

		const FString HP = FString::Printf(TEXT("HP : %d"), MyCharacter->GetHP());
		DrawText(HP, FColor::Orange, Canvas->OrgX+50, Canvas->ClipY-75, HudFont);
		
		
	}

	const FVector2D CrosshairDrawPosition( (Center.X),(Center.Y + 20.0f));

	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

