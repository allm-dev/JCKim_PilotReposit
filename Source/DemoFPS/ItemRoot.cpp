// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemRoot.h"
#include "AmmunitionComp.h"
#include "DemoFPSCharacter.h"
#include "DemoFPSGameInstance.h"
#include "DemoFPSPlayerState.h"

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

	bReplicates = true;
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
			OtherCharacter->SetWeaponInSlot(EWeaponClassKey::WeaponClass1);
			break;
		case EItemType::Weapon1:
			OtherCharacter->SetWeaponInSlot(EWeaponClassKey::WeaponClass2);
			break;
		case EItemType::Weapon2:
			OtherCharacter->SetWeaponInSlot(EWeaponClassKey::WeaponClass3);
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
			ExecuteImmediateHeal(50, OtherCharacter);
			break;
		case EItemType::HealPackHuge:
			ExecuteImmediateHeal(100, OtherCharacter);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Invalid Item Type to Spawn"));
			break;
		}
		
		Destroy();
	}
}

void AItemRoot::GenerateRandAmmoRoot(int32 WeaponAmmoId, int32 Min, int32 Max, ADemoFPSCharacter* Receiver) const
{
	if (!IsValid(Receiver))
	{
		return;
	}
	UAmmunitionComp* AmmunitionBag = Receiver->GetAmmunitionComp();

	if (AmmunitionBag != nullptr)
	{
		AmmunitionBag->AddAmmoXCount(WeaponAmmoId, FMath::RandRange(Min, Max));
	}
}

void AItemRoot::GenerateRandGrenadeRoot(int32 Min, int32 Max, ADemoFPSCharacter* Receiver) const
{
	if (!IsValid(Receiver))
	{
		return;
	}
	UAmmunitionComp* AmmunitionBag = Receiver->GetAmmunitionComp();

	if (AmmunitionBag != nullptr)
	{
		AmmunitionBag->AddCurrentGrenadeCount(FMath::RandRange(Min, Max));
	}
}

void AItemRoot::ExecuteImmediateHeal(uint8 HealAmount, ADemoFPSCharacter* Receiver) const
{
	if (!IsValid(Receiver))
	{
		return;
	}
	ADemoFPSPlayerState* MyplayerState = Receiver->GetMyPlayerState();

	if(IsValid(MyplayerState))
	{
		MyplayerState->AddCurrentHP(HealAmount);
	}
}