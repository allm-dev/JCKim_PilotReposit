// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "DemoFPSProjectile.h"
#include "DemoFPSCharacter.h"



// Sets default values
AWeapon::AWeapon()
{
	WeaponName = TEXT("기본 소총");
	
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	Mesh->SetOnlyOwnerSee(true);			
	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = false;
	RootComponent = Mesh;

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	Muzzle->SetupAttachment(Mesh);
	Muzzle->SetRelativeLocation(FVector(0.0f, 60.0f, 11.0f));
	
	MaxAmmo =30;
	CurrentAmmo = MaxAmmo;
}



bool AWeapon::FireGun()
{
	if (CurrentAmmo <= 0)
	{
		return false;
	}

	if (ProjectileClass == nullptr)
	{
		return false;
	}
	
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return false;
	}
	
	ADemoFPSCharacter* GunOwner = Cast<ADemoFPSCharacter>(GetOwner());
	if (GunOwner == nullptr)
	{
		return false;
	}
	
	GunOwner->AddControllerPitchInput(FMath::RandRange(-1.50f, 0.00f));
	GunOwner->AddControllerYawInput(FMath::RandRange(-1.00f, 1.00f));

	const FRotator SpawnRotation = GunOwner->GetControlRotation();
	const FVector SpawnLocation = Muzzle->GetComponentLocation();

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	ActorSpawnParams.Owner = GunOwner;

	ADemoFPSProjectile* Projectile = World->SpawnActor<ADemoFPSProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	if (!IsValid(Projectile))
	{
		return false;
	}

	Projectile->OnGSWKill.AddUObject(GunOwner, &ADemoFPSCharacter::AddKillScore);
	CurrentAmmo--;

	return true;
}

bool AWeapon::Reload(int32 NewAmmo)
{
	if (CurrentAmmo >= MaxAmmo || NewAmmo <= 0)
	{
		return false;
	}
	
	CurrentAmmo += NewAmmo;
	return true;
}





