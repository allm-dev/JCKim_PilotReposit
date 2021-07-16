// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoFPSCharacter.h"

#include "DemoFPSGameInstance.h"
#include "DemoFPSHUD.h"
#include "DemoFPSProjectile.h"
#include "Grenade.h"
#include "ItemRoot.h"
#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "MovieSceneTracksComponentTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ADemoFPSCharacter

ADemoFPSCharacter::ADemoFPSCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(7.0f, -15.0f, 3.0f));
	Mesh1P->SetRelativeLocation(FVector(15.0f,-15.0f, -165.0f));

	MaxWeaponSlots = 3;
	WeaponInventory.Init(nullptr, MaxWeaponSlots);
	CurrentWeapon = nullptr;
	
	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	//Aim Camera
	AimCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("AimCamera"));
	AimCamera->SetupAttachment(FirstPersonCameraComponent);
	AimCamera->SetRelativeRotation(FRotator(0,0,0));
	AimCamera->SetRelativeLocation(FVector(100,0,0));
	AimCamera->SetCameraMesh(nullptr);
	AimCamera->bUsePawnControlRotation = true;
	AimCamera->SetActive(false);

	static ConstructorHelpers::FClassFinder<AWeapon> DefaultGunBP(TEXT("/Game/FirstPersonCPP/Blueprints/Weapon1.Weapon1_C"));
	DefaultGunClass = DefaultGunBP.Class;

	static ConstructorHelpers::FClassFinder<AWeapon> DefaultGunBP2(TEXT("/Game/FirstPersonCPP/Blueprints/Weapon2.Weapon2_C"));
	DefaultGunClass2 = DefaultGunBP2.Class;

	static ConstructorHelpers::FClassFinder<AWeapon> DefaultGunBP3(TEXT("/Game/FirstPersonCPP/Blueprints/Weapon3.Weapon3_C"));
	DefaultGunClass3 = DefaultGunBP3.Class;

	Ammo0Count =30;
	Ammo1Count = 7;
	Ammo2Count = 10;
	GrenadeCount = 3;

	CurrentHP = 100;
}

void ADemoFPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BombClass = AGrenade::StaticClass();
	
	auto World = GetWorld();
	if(IsValid(World))
	{
		if(IsValid(DefaultGunClass))
		{
			auto* const NewWeapon = World->SpawnActor<AWeapon>(DefaultGunClass, FVector::ZeroVector, FRotator::ZeroRotator);
			if(IsValid(NewWeapon))
			{
				SetWeapon(NewWeapon);
			}
		}
		if(IsValid(DefaultGunClass2))
		{
			auto* const NewWeapon = World->SpawnActor<AWeapon>(DefaultGunClass2, FVector::ZeroVector, FRotator::ZeroRotator);
			if(IsValid(NewWeapon))
			{
				SetWeapon(NewWeapon);
			}
		}
		if(IsValid(DefaultGunClass3))
		{
			auto* const NewWeapon = World->SpawnActor<AWeapon>(DefaultGunClass3, FVector::ZeroVector, FRotator::ZeroRotator);
			if(IsValid(NewWeapon))
			{
				SetWeapon(NewWeapon);
			}
		}

		EquipSlot1();
	}
}

void ADemoFPSCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{   
	for(int i =0; i<MaxWeaponSlots; i++)
	{
		if(WeaponInventory[i] !=nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Clear Inventory of %s"), *GetName());
			WeaponInventory[i]->Destroy();
		}
	}
	Super::EndPlay(EndPlayReason);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADemoFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ADemoFPSCharacter::OnFireWeapon);

	//폭격 액션
	PlayerInputComponent->BindAction("Bomb", IE_Pressed, this, &ADemoFPSCharacter::OnBomb);

	//조준
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ADemoFPSCharacter::AimOn);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ADemoFPSCharacter::AimOff);

	//무기교체
	PlayerInputComponent->BindAction("EquipSlot1", IE_Pressed, this, &ADemoFPSCharacter::EquipSlot1);
	PlayerInputComponent->BindAction("EquipSlot2", IE_Pressed, this, &ADemoFPSCharacter::EquipSlot2);
	PlayerInputComponent->BindAction("EquipSlot3", IE_Pressed, this, &ADemoFPSCharacter::EquipSlot3);
	
	//장전
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ADemoFPSCharacter::ReloadWeapon);

	//재시작
	PlayerInputComponent->BindAction("Restart", IE_Pressed, this, &ADemoFPSCharacter::RestartGame).bExecuteWhenPaused =true;

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ADemoFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADemoFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADemoFPSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADemoFPSCharacter::LookUpAtRate);
}

void ADemoFPSCharacter::SetWeapon(AWeapon* NewWeapon)
{
	if(!IsValid(NewWeapon))
	{
		return;
	}

	if(IsValid(CurrentWeapon))
	{
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	NewWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	NewWeapon->SetOwner(this);
	
	for(int i =0; i<MaxWeaponSlots; i++)
	{
		if(!IsValid(WeaponInventory[i]))
		{
			WeaponInventory[i] = NewWeapon;
			CurrentWeapon = NewWeapon;
			return;
		}
	}

	//Full inventory
	for(int i =0; i<MaxWeaponSlots; i++)
	{
		if(WeaponInventory[i] == CurrentWeapon)
		{

			int32 const PrevWeaponId = CurrentWeapon->GetWeaponId();
			CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			CurrentWeapon->SetOwner(nullptr);
			CurrentWeapon->Destroy();
			
			//UClass* SpawnClass = StaticLoadClass(AItemRoot::StaticClass(), nullptr, *FString::Printf(TEXT("/Game/FirstPersonCPP/Blueprints/Weapon%dRoot.Weapon%dRoot_C"), PrevWeaponId, PrevWeaponId));
			//GetWorld()->SpawnActor<AItemRoot>(SpawnClass, GetActorLocation()+GetActorForwardVector()*250, FRotator::ZeroRotator);

			UClass* SpawnClass = GetGameInstance<UDemoFPSGameInstance>()->GetItemRootClass()[static_cast<ItemRootClassKey>(PrevWeaponId)];
			if (SpawnClass != nullptr)
			{
				GetWorld()->SpawnActor<AItemRoot>(SpawnClass, GetActorLocation()+GetActorForwardVector()*250, FRotator::ZeroRotator);
			}
			CurrentWeapon = NewWeapon;
			WeaponInventory[i] = NewWeapon;
		}
	}
	
}

int32 ADemoFPSCharacter::GetAmmoCount() const
{
	int32 Res =0;
	
	if(CurrentWeapon != nullptr)
	switch (CurrentWeapon->GetAmmoId())
	{
	case 0:
		Res =Ammo0Count;
		break;
	case 1:
		Res = Ammo1Count;
		break;
	case 2:
		Res=Ammo2Count;
		break;
	default:
		Res =0;
		break;
	}
	
	return Res;
}

void ADemoFPSCharacter::SetAmmoCountUp(int32 AmmoId)
{
	switch (AmmoId)
	{
		case 0:
			Ammo0Count += FMath::RandRange(40,60);
		break;
		
		case 1:
			Ammo1Count += FMath::RandRange(10,20);
		break;

		case 2:
			Ammo2Count += FMath::RandRange(15,25);
		break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("Unknown Ammo ID, Discarded"));
			break;
	}
}

void ADemoFPSCharacter::ReloadWeapon()
{
	int32 NewAmmo = CurrentWeapon->NeedAmmo();

	switch (CurrentWeapon->GetAmmoId())
	{
		case 0:
			if(NewAmmo > Ammo0Count) NewAmmo = Ammo0Count;
			Ammo0Count -= NewAmmo;
			break;
		case 1:
			if(NewAmmo > Ammo1Count) NewAmmo = Ammo1Count;
			Ammo1Count -= NewAmmo;
			break;
		case 2:
			if(NewAmmo > Ammo2Count) NewAmmo = Ammo2Count;
			Ammo2Count -= NewAmmo;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Unknown AmmoId"));
			NewAmmo =0;
			break;
	}

	CurrentWeapon->Reload(NewAmmo);
	
}

void ADemoFPSCharacter::EquipSlot1()
{
	if(WeaponInventory[0] !=nullptr && CurrentWeapon != WeaponInventory[0])
	{
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon = WeaponInventory[0];
		CurrentWeapon->SetActorHiddenInGame(false);
	}
}

void ADemoFPSCharacter::EquipSlot2()
{
	if(WeaponInventory[1] !=nullptr && CurrentWeapon != WeaponInventory[1])
	{
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon = WeaponInventory[1];
		CurrentWeapon->SetActorHiddenInGame(false);
	}
}

void ADemoFPSCharacter::EquipSlot3()
{
	if(WeaponInventory[2] !=nullptr && CurrentWeapon != WeaponInventory[2])
	{
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon = WeaponInventory[2];
		CurrentWeapon->SetActorHiddenInGame(false);
	}
}

void ADemoFPSCharacter::RestartGame()
{
	
	APlayerController* PlayerController = GetController<APlayerController>();
	if(IsValid(PlayerController))
	{
		
		auto const MyHUD = PlayerController->GetHUD<ADemoFPSHUD>();
		if(MyHUD != nullptr)
		{
			if(MyHUD->GetGameOver() == true)
			{
				UGameplayStatics::OpenLevel(GetWorld(), TEXT("FirstPersonExampleMap"));
			}
		}
	}
}

void ADemoFPSCharacter::AddDamage(int32 NewDamage)
{
	CurrentHP = FMath::Clamp<int32>(CurrentHP - NewDamage, 0 , 100);
	UE_LOG(LogTemp, Warning, TEXT("%s got %d damage"), *GetName(), NewDamage)
	if(CurrentHP <= 0)
	{
		Destroy();
	}
}

void ADemoFPSCharacter::OnFireWeapon()
{
	if(CurrentWeapon !=nullptr)
	{
		if(CurrentWeapon->FireGun())
		{
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			if (FireAnimation != nullptr)
			{
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != nullptr)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}
		}
	}
}

void ADemoFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ADemoFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ADemoFPSCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADemoFPSCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ADemoFPSCharacter::AimOn()
{
	UE_LOG(LogTemp, Warning, TEXT("AimOn"));
	FirstPersonCameraComponent->SetActive(false);
	AimCamera->SetActive(true);
	AimCamera->AttachToComponent(CurrentWeapon->GetMuzzle(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
}

void ADemoFPSCharacter::AimOff()
{
	UE_LOG(LogTemp, Warning, TEXT("AimOff"));
	FirstPersonCameraComponent->SetActive(true);
	AimCamera->AttachToComponent(FirstPersonCameraComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
	AimCamera->SetRelativeLocation(GunOffset);
	AimCamera->SetActive(false);
}

void ADemoFPSCharacter::OnBomb()
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}
	
	if(BombClass == nullptr || GrenadeCount  <= 0)
	{
		return;
	}
	
	const FRotator SpawnRotation = GetControlRotation();
	const FVector SpawnLocation =  GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	auto* Grenade = World->SpawnActor<AGrenade>(BombClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	if (!IsValid(Grenade))
	{
		return;
	}
	
	Grenade->SetOwner(this);
	Grenade->OnGrenadeKill.BindUObject(this, &ADemoFPSCharacter::AddKillScore);
	GrenadeCount--;
	UE_LOG(LogTemp, Warning, TEXT("Bombing Engaged"));
}


