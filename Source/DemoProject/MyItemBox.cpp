// Fill out your copyright notice in the Description page of Project Settings.


#include "MyItemBox.h"
#include "MyCharacter.h"
#include "MyWeapon.h"

#include <corecrt_io.h>

// Sets default values
AMyItemBox::AMyItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));

	//collision comp takes the role of root comp
	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	//extend val , 3d center vec of box collision zone 
	Trigger->SetBoxExtent(FVector(40,42,30));
	//static mesh val
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1"));
	if(SM_BOX.Succeeded())
	{
		Box->SetStaticMesh(SM_BOX.Object);
	}
	//Particle system val
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_EFFECT(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));
	if(PS_EFFECT.Succeeded())
	{
		Effect->SetTemplate(PS_EFFECT.Object);
		// Effect->SetAutoActivate(false); Any difference with this code line???
		Effect->bAutoActivate = false;
	}

	//loc of box(inferior comp loc setup)
	Box->SetRelativeLocation(FVector(0.0f,-3.5f,-30.0f));

	//collision profile setup
	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	Box->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponItemClass = AMyWeapon::StaticClass();
}

// Called when the game starts or when spawned
void AMyItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//register audience to delegate
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AMyItemBox::OnCharacterOverlap);
	
}

//audience driver code
void AMyItemBox::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABLOG_S(Warning);
	auto MyCharacter = Cast<AMyCharacter>(OtherActor);
	MYCHECK(nullptr != MyCharacter);

	//character exist + weapon exist
	if(MyCharacter != nullptr && WeaponItemClass !=nullptr)
	{
		//character hand is empty
		if(MyCharacter->CanSetWeapon())
		{
			//spawn new weapon
			auto NewWeapon = GetWorld()->SpawnActor<AMyWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
			//equipping driver code on
			MyCharacter->SetWeapon(NewWeapon);

			//effect driver codes
			Effect->Activate(true);
			Box->SetHiddenInGame(true, true);
			SetActorEnableCollision(false);
			//upon the end of ps effect play
			Effect->OnSystemFinished.AddDynamic(this, &AMyItemBox::OnEffectFinished);
		}
		//character is holding something else already
		else
		{
			//log detail reason
			ABLOG(Warning, TEXT("%s has already equipped other weapon on hand"), *MyCharacter->GetName());
		}
	}
}

//do this upon the end of box-open effect play
void AMyItemBox::OnEffectFinished(UParticleSystemComponent* ParticleSys)
{
	Destroy();
}



