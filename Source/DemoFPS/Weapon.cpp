// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "DemoFPSProjectile.h"
#include "DemoFPSCharacter.h"
#include "DemoFPSPlayerState.h"
#include "Net/UnrealNetwork.h"


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

	GunOwner = nullptr;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, GunOwner);
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

	ADemoFPSCharacter* PlayerPawn = GetOwner<ADemoFPSCharacter>();
	if (!IsValid(PlayerPawn))
	{
		return false;
	}

	const FRotator SpawnRotation = PlayerPawn->GetControlRotation();
	const FVector SpawnLocation = Muzzle->GetComponentLocation();

	ServerFireGun(SpawnLocation, SpawnRotation);
	
	CurrentAmmo--;

	return true;
}


bool AWeapon::ServerFireGun_Validate(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if(SpawnLocation == FVector(ForceInit) || SpawnRotation == FRotator(ForceInit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong Spawn Point Coordination Assigned"));
		return false;
	}
	
	return true;
}


void AWeapon::ServerFireGun_Implementation(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	UE_LOG(LogTemp, Warning, TEXT("Executing ServerFireGun"))
	SpawnBullet(SpawnLocation, SpawnRotation);
	MultiCastFireGun();
}

void AWeapon::SpawnBullet(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	UWorld* const World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("No World Found on Server"))
		return;
	}
	
	GunOwner = GetOwner<ADemoFPSCharacter>();
	if (!IsValid(GunOwner))
	{
		UE_LOG(LogTemp, Warning, TEXT("No GunOwner on Server"))
		return;
	}
	
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	ActorSpawnParams.Owner = GunOwner;

	ADemoFPSProjectile* Projectile = World->SpawnActor<ADemoFPSProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	if (!IsValid(Projectile))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed Spawning bullet on Server"))
		return;
	}

	ADemoFPSPlayerState* MyPlayerState = GunOwner->GetMyPlayerState();
	check(IsValid(MyPlayerState))
	if (IsValid(MyPlayerState))
	{
		Projectile->OnGunShotKill.AddDynamic(MyPlayerState,&ADemoFPSPlayerState::AddKillScore);
	}
}

void AWeapon::MultiCastFireGun_Implementation()
{
	GunOwner = GetOwner<ADemoFPSCharacter>();
	if (!IsValid(GunOwner))
	{
		UE_LOG(LogTemp, Warning, TEXT("No GunOwner on Client"))
		return;
	}
	
	GunOwner->AddControllerPitchInput(FMath::RandRange(-1.50f, 0.00f));
	GunOwner->AddControllerYawInput(FMath::RandRange(-1.00f, 1.00f));
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





