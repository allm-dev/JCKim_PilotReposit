// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemRoot.h"
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
	auto OtherCharacter = Cast<ADemoFPSCharacter>(OtherActor);
	if (IsValid(OtherCharacter))
	{
		switch (ItemType)
		{
		case EItemType::Weapon0:
			if(ItemPathFinder.Contains(EItemType::Weapon0))
			{
				UClass* NewWeaponClass = ItemPathFinder[EItemType::Weapon0];
				if(NewWeaponClass != nullptr)
				{
					AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(NewWeaponClass);
					if(NewWeapon != nullptr)
					{
						OtherCharacter->SetWeapon(NewWeapon);
					}
				}
			}
				//AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(StaticLoadClass(AWeapon::StaticClass(), nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/Weapon1.Weapon1_C")));
				//check(NewWeapon !=nullptr)
				//OtherCharacter->SetWeapon(NewWeapon);
			break;
		case EItemType::Weapon1:
			if(ItemPathFinder.Contains(EItemType::Weapon1))
			{
				UClass* NewWeaponClass = ItemPathFinder[EItemType::Weapon1];
				if(NewWeaponClass != nullptr)
				{
					AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(NewWeaponClass);
					if(NewWeapon != nullptr)
					{
						OtherCharacter->SetWeapon(NewWeapon);
					}
				}
			}
				//AWeapon* NewWeapon1 = GetWorld()->SpawnActor<AWeapon>(StaticLoadClass(AWeapon::StaticClass(), nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/Weapon2.Weapon2_C")));
				//check(NewWeapon1 !=nullptr)
				//OtherCharacter->SetWeapon(NewWeapon1);
			break;
		case EItemType::Weapon2:
			if(ItemPathFinder.Contains(EItemType::Weapon2))
			{
				UClass* NewWeaponClass = ItemPathFinder[EItemType::Weapon2];
				if(NewWeaponClass != nullptr)
				{
					AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(NewWeaponClass);
					if(NewWeapon != nullptr)
					{
						OtherCharacter->SetWeapon(NewWeapon);
					}
				}
			}
				//AWeapon* NewWeapon2 = GetWorld()->SpawnActor<AWeapon>(StaticLoadClass(AWeapon::StaticClass(), nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/Weapon3.Weapon3_C")));
				//check(NewWeapon2 !=nullptr)
				//OtherCharacter->SetWeapon(NewWeapon2);
			break;
		case EItemType::Ammo0:
			OtherCharacter->SetAmmoCountUp(0);
			break;
		case EItemType::Ammo1:
			OtherCharacter->SetAmmoCountUp(1);
			break;
		case EItemType::Ammo2:
			OtherCharacter->SetAmmoCountUp(2);
			break;
		case EItemType::Grenade:
			OtherCharacter->SetGrenadeCountUp();
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
