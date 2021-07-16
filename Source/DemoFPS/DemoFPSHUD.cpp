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

	bGameOver = false;

}

void ADemoFPSHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	ADemoFPSGameMode* MyGameMode = Cast<ADemoFPSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	MyGameMode->OnGameOver.BindUObject(this, &ADemoFPSHUD::SetGameOver);
}


void ADemoFPSHUD::DrawHUD()
{
	Super::DrawHUD();

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	ADemoFPSCharacter* MyCharacter = Cast<ADemoFPSCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	ADemoFPSGameMode* MyGameMode = Cast<ADemoFPSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if(bGameOver)
	{
		const FString GameOver = FString::Printf(TEXT("Game Over"));
		DrawText(GameOver, FColor::Turquoise, Center.X-100, Center.Y-50, HudFont, 2, false);

		const FString KillScore = FString::Printf(TEXT("Kill Score : %d"), MyCharacter->GetKillScore());
		DrawText(KillScore, FColor::Yellow, Center.X-50, Center.Y+50, HudFont);

		const FString Reset = FString::Printf(TEXT("Press P to Restart"));
		DrawText(Reset, FColor::White, Center.X, Canvas->ClipY-120, HudFont, 1, false);

		return;
	}

	if(MyCharacter != nullptr && MyGameMode != nullptr)
	{
		const FString KillScore = FString::Printf(TEXT("Kill Score : %d"), MyCharacter->GetKillScore());
		DrawText(KillScore, FColor::Yellow, Center.X + 100, Center.Y-Canvas->ClipY*0.45f, HudFont);

		const FString RunTimeCheck = FString::Printf(TEXT("Remaining time : %.2f"), MyGameMode->GetRemainingTime());
		DrawText(RunTimeCheck, FColor::Red, Center.X-200, Center.Y-Canvas->ClipY*0.45f, HudFont);

		if(MyCharacter->GetCurrentWeapon() != nullptr)
		{
			AWeapon* MyWeapon = MyCharacter->GetCurrentWeapon();

			const FString WeaponName = FString::Printf(TEXT("장착 무기 : %s"), *MyWeapon->GetName());
			DrawText(WeaponName, FColor::Blue, Canvas->ClipX-200, Canvas->ClipY-115, HudFont);

			const FString AmmunitionStat = FString::Printf(TEXT("Ammo : %d / %d"), MyWeapon->GetCurrentAmmo(), MyCharacter->GetAmmoCount());
			DrawText(AmmunitionStat, FColor::White,Canvas->ClipX-200, Canvas->ClipY-75, HudFont);
		}
		
		const FString GrenadeStat = FString::Printf(TEXT("Grenade : %d"), MyCharacter->GetGrenadeCount());
		DrawText(GrenadeStat, FColor::Silver,Canvas->ClipX-200, Canvas->ClipY-35, HudFont);

		const FString HP = FString::Printf(TEXT("HP : %d"), MyCharacter->GetCurrentHP());
		DrawText(HP, FColor::Orange, Canvas->OrgX+50, Canvas->ClipY-75, HudFont);
		
	}

	const FVector2D CrosshairDrawPosition( (Center.X),(Center.Y + 20.0f));

	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

