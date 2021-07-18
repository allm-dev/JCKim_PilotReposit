// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemRoot.h"

#include "AmmunitionComp.h"
#include "Weapon.h"
#include "DemoFPSCharacter.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

// Sets default values
AItemRoot::AItemRoot()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AItemRoot::OnBeginOverlap);
	
	MeshCompST = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshST"));
	MeshCompSK = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshSK"));

	RootComponent = SphereComp;
	MeshCompST->SetupAttachment(RootComponent);
	MeshCompSK->SetupAttachment(RootComponent);

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for(int i = 0; i<EItemType::End; i++)
	{
		ItemPathFinder.Add(static_cast<EItemType>(i), nullptr);
	}
}

void AItemRoot::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADemoFPSCharacter* OtherCharacter = Cast<ADemoFPSCharacter>(OtherActor);
	
	if (IsValid(OtherCharacter))
	{

		switch (ItemType)
		{
		case EItemType::Weapon0:
			OtherCharacter->SetWeaponInSlot(GenerateWeaponRoot(Weapon0));
			break;
		case EItemType::Weapon1:
			OtherCharacter->SetWeaponInSlot(GenerateWeaponRoot(Weapon1));
			break;
		case EItemType::Weapon2:
			OtherCharacter->SetWeaponInSlot(GenerateWeaponRoot(Weapon2));
			break;
		case EItemType::Ammo0:
			GenerateRandAmmoRoot(0, 20, 40, OtherCharacter);
			break;
		case EItemType::Ammo1:
			GenerateRandAmmoRoot(1, 5, 10, OtherCharacter);
			break;
		case EItemType::Ammo2:
			GenerateRandAmmoRoot(2, 7, 14, OtherCharacter);
			break;
		case EItemType::Grenade:
			GenerateRandGrenadeRoot(3,6, OtherCharacter);
			break;
		case EItemType::HealPackSmall:
			OtherCharacter->AddCurrentHP(50);
			break;
		case EItemType::HealPackHuge:
			OtherCharacter->AddCurrentHP(100);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Invalid Item Type to Spawn"));
			break;
		}
		
		Destroy();
	}
}

AWeapon* AItemRoot::GenerateWeaponRoot(EItemType NewItemType)
{
	if (!ItemPathFinder.Contains(NewItemType))
	{
		return nullptr;
	}
	
	UClass* NewWeaponClass = ItemPathFinder[NewItemType];
	if (NewWeaponClass == nullptr)
	{
		return  nullptr;
	}
	
	AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(NewWeaponClass);
	if (IsValid(NewWeapon))
	{
		return NewWeapon;
	}
	else
	{
		return nullptr;
	}
}

void AItemRoot::GenerateRandAmmoRoot(int32 WeaponAmmoId, int32 Min, int32 Max, ADemoFPSCharacter* Receiver)
{
	UAmmunitionComp* AmmunitionBag = Receiver->GetAmmunitionComp();

	if (AmmunitionBag != nullptr)
	{
		AmmunitionBag->AddAmmoXCount(WeaponAmmoId, FMath::RandRange(Min, Max));
	}
}

void AItemRoot::GenerateRandGrenadeRoot(int32 Min, int32 Max, ADemoFPSCharacter* Receiver)
{
	UAmmunitionComp* AmmunitionBag = Receiver->GetAmmunitionComp();

	if (AmmunitionBag != nullptr)
	{
		AmmunitionBag->AddCurrentGrenadeCount(FMath::RandRange(Min, Max));
	}
}