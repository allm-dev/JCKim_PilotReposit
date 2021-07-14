// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemRoot.h"
#include "Weapon.h"
#include "DemoFPSCharacter.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

// Sets default values
AItemRoot::AItemRoot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	MeshCompST = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshST"));
	MeshCompSK = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshSK"));

	RootComponent = SphereComp;
	MeshCompST->SetupAttachment(RootComponent);
	MeshCompSK->SetupAttachment(RootComponent);

	SphereComp->SetCollisionProfileName("OverlapOnlyPawn");
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AItemRoot::OnBeginOverlap);

}

// Called when the game starts or when spawned
void AItemRoot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemRoot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

inline void AItemRoot::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto const OtherCharacter = Cast<ADemoFPSCharacter>(OtherActor);
	if(OtherCharacter != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Collision Checked"));
		
		UClass* Weapon0Path = StaticLoadClass(AWeapon::StaticClass(), nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/Weapon1.Weapon1_C"));
		UClass* Weapon1Path = StaticLoadClass(AWeapon::StaticClass(), nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/Weapon2.Weapon2_C"));
		UClass* Weapon2Path = StaticLoadClass(AWeapon::StaticClass(), nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/Weapon3.Weapon3_C"));
		switch (ItemType)
		{
			case EItemType::Weapon0:
				
				if(Weapon0Path!=nullptr)
				{
					auto NewWeapon = GetWorld()->SpawnActor<AWeapon>(Weapon0Path);
					check(NewWeapon !=nullptr)
					OtherCharacter->SetWeapon(NewWeapon);
				}
				break;
			case EItemType::Weapon1:
				
				if(Weapon1Path!=nullptr)
				{
					auto NewWeapon = GetWorld()->SpawnActor<AWeapon>(Weapon1Path);
					check(NewWeapon !=nullptr)
					OtherCharacter->SetWeapon(NewWeapon);
				}
				break;
			case EItemType::Weapon2:
				
				if(Weapon2Path!=nullptr)
				{
					auto NewWeapon = GetWorld()->SpawnActor<AWeapon>(Weapon2Path);
					check(NewWeapon !=nullptr)
					OtherCharacter->SetWeapon(NewWeapon);
				}
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
				OtherCharacter->SetHPUp(50);
				break;
			case EItemType::HealPackHuge:
				OtherCharacter->SetHPUp(100);
				break;
			default:
				UE_LOG(LogTemp, Warning, TEXT("Invalid Item Type to Spawn"));
				break;
		}
		
		Destroy();
	}
}
