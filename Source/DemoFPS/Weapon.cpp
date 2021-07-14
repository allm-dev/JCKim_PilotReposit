// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "DemoFPSProjectile.h"
#include "DemoFPSCharacter.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	Mesh->SetOnlyOwnerSee(false);			
	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = false;
	Mesh->SetOnlyOwnerSee(true);
	RootComponent = Mesh;

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	Muzzle->SetupAttachment(Mesh);
	Muzzle->SetRelativeLocation(FVector(0.0f, 60.0f, 11.0f));

	MaxAmmo =30;
	CurrentAmmo = MaxAmmo;
}

bool AWeapon::FireGun()
{
	// try and fire a projectile
	if (ProjectileClass != nullptr && CurrentAmmo >0)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			auto GunOwner = Cast<ADemoFPSCharacter>(GetOwner());
			check(GunOwner !=nullptr);
			//총기 반동
			GunOwner->AddControllerPitchInput(FMath::RandRange(-1.50f, 0.00f));
			GunOwner->AddControllerYawInput(FMath::RandRange(-1.00f, 1.00f));
			
			const FRotator SpawnRotation = GunOwner->GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = Muzzle->GetComponentLocation();

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<ADemoFPSProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			return true;
		}
	}

	return false;
}

bool AWeapon::Reload(int32 NewAmmo)
{
	if(CurrentAmmo < MaxAmmo)
	{
		CurrentAmmo += NewAmmo;
		return true;
	}
	return false;
}


// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}



