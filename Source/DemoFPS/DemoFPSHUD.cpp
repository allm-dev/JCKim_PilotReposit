// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoFPSHUD.h"
#include "AmmunitionComp.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"
#include "DemoFPSCharacter.h"
#include "DemoFPSPlayerState.h"
#include "DemoFPSGameState.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#undef DrawText

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

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	ADemoFPSGameState* GameState = World->GetGameState<ADemoFPSGameState>();
	if (!IsValid(GameState))
	{
		UE_LOG(LogTemp, Warning, TEXT("No gamestate loaded"));
		return;
	}

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(PC))
	{
		return;
	}
	
	ADemoFPSPlayerState* MyPlayerState = PC->GetPlayerState<ADemoFPSPlayerState>();
	if (!IsValid(MyPlayerState))
	{
		return;
	}
	ADemoFPSCharacter* MyCharacter = PC->GetPawn<ADemoFPSCharacter>();

	
	if (GameState->GetIsGameOver())
	{
		const FString GameOver = FString::Printf(TEXT("Game Over"));
		DrawText(GameOver, FColor::Turquoise, Center.X-100, Center.Y-50, HudFont, 2, false);

		const FString KillScore = FString::Printf(TEXT("Kill Score : %d"), MyPlayerState->GetKillScore());
		DrawText(KillScore, FColor::Yellow, Center.X-50, Center.Y+50, HudFont);
		
		const FString Reset = FString::Printf(TEXT("Press P to Restart"));
		DrawText(Reset, FColor::White, Center.X, Canvas->ClipY-120, HudFont, 1, false);

		return;
	}

	const FString RemainingTimeCheck = FString::Printf(TEXT("Remaining time : %.2f"), GameState->GetRemainingTime());
	DrawText(RemainingTimeCheck, FColor::Red, Center.X-200, Center.Y-Canvas->ClipY*0.45f, HudFont);

	const FString KillScore = FString::Printf(TEXT("Kill Score : %d"), MyPlayerState->GetKillScore());
	DrawText(KillScore, FColor::Yellow, Center.X + 100, Center.Y-Canvas->ClipY*0.45f, HudFont);
	
	if (IsValid(MyCharacter))
	{
		AWeapon* MyWeapon = MyCharacter->GetCurrentWeapon();
		if (IsValid(MyWeapon))
		{
			const FString WeaponName = FString::Printf(TEXT("장착 무기 : %s"), *MyWeapon->GetWeaponName());
			DrawText(WeaponName, FColor::Blue, Canvas->ClipX-200, Canvas->ClipY-115, HudFont);

			UAmmunitionComp* AmmunitionComp = MyCharacter->GetAmmunitionComp();
			if(AmmunitionComp != nullptr)
			{
				const FString AmmunitionStat = FString::Printf(TEXT("Ammo : %d / %d"), MyWeapon->GetCurrentAmmo(), AmmunitionComp->GetAmmoXCount(MyWeapon->GetAmmoId()));
				DrawText(AmmunitionStat, FColor::White,Canvas->ClipX-200, Canvas->ClipY-75, HudFont);
				
				const FString GrenadeStat = FString::Printf(TEXT("Grenade : %d"), AmmunitionComp->GetCurrentGrenadeCount());
				DrawText(GrenadeStat, FColor::Silver,Canvas->ClipX-200, Canvas->ClipY-35, HudFont);
			}
		}
		
		const FString HP = FString::Printf(TEXT("HP : %d"), MyPlayerState->GetCurrentHP());
		DrawText(HP, FColor::Orange, Canvas->OrgX+50, Canvas->ClipY-75, HudFont);
	}

	const FVector2D CrosshairDrawPosition( (Center.X),(Center.Y + 20.0f));

	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

