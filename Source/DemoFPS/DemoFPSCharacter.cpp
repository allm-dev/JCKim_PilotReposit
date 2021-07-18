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
#include "AmmunitionComp.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

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

	AmmunitionBag = CreateDefaultSubobject<UAmmunitionComp>(TEXT("AmmunitionBag"));

	/*
	Ammo0Count =30;
	Ammo1Count = 7;
	Ammo2Count = 10;
	GrenadeCount = 3;
	*/

	CurrentHP = 100;
}

void ADemoFPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BombClass = AGrenade::StaticClass();
	check(IsValid(BombClass));
	
	auto World = GetWorld();
	if (IsValid(World))
	{
		if (IsValid(DefaultGunClass))
		{
			AWeapon* const NewWeapon = World->SpawnActor<AWeapon>(DefaultGunClass, FVector::ZeroVector, FRotator::ZeroRotator);
			if (IsValid(NewWeapon))
			{
				SetWeaponInSlot(NewWeapon);
			}
		}
		if (IsValid(DefaultGunClass2))
		{
			AWeapon* const NewWeapon = World->SpawnActor<AWeapon>(DefaultGunClass2, FVector::ZeroVector, FRotator::ZeroRotator);
			if (IsValid(NewWeapon))
			{
				SetWeaponInSlot(NewWeapon);
			}
		}
		if (IsValid(DefaultGunClass3))
		{
			AWeapon* const NewWeapon = World->SpawnActor<AWeapon>(DefaultGunClass3, FVector::ZeroVector, FRotator::ZeroRotator);
			if (IsValid(NewWeapon))
			{
				SetWeaponInSlot(NewWeapon);
			}
		}

		EquipSlot1();
	}
}

void ADemoFPSCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{   
	for (int i =0; i<MaxWeaponSlots; i++)
	{
		if (WeaponInventory.IsValidIndex(i) && IsValid(WeaponInventory[i]))
		{
			UE_LOG(LogTemp, Warning, TEXT("Clear Inventory of %s"), *GetName());
			WeaponInventory[i]->Destroy();
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ADemoFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ADemoFPSCharacter::OnFireWeapon);

	PlayerInputComponent->BindAction("Bomb", IE_Pressed, this, &ADemoFPSCharacter::OnBombKeyPressed);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ADemoFPSCharacter::AimOn);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ADemoFPSCharacter::AimOff);

	PlayerInputComponent->BindAction("EquipSlot1", IE_Pressed, this, &ADemoFPSCharacter::EquipSlot1);
	PlayerInputComponent->BindAction("EquipSlot2", IE_Pressed, this, &ADemoFPSCharacter::EquipSlot2);
	PlayerInputComponent->BindAction("EquipSlot3", IE_Pressed, this, &ADemoFPSCharacter::EquipSlot3);
	
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ADemoFPSCharacter::ReloadWeapon);

	PlayerInputComponent->BindAction("Restart", IE_Pressed, this, &ADemoFPSCharacter::RestartGame).bExecuteWhenPaused =true;

	PlayerInputComponent->BindAxis("MoveForward", this, &ADemoFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADemoFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADemoFPSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADemoFPSCharacter::LookUpAtRate);
}

void ADemoFPSCharacter::SetWeaponInSlot(AWeapon* NewWeapon)
{
	if (!IsValid(NewWeapon))
	{
		return;
	}

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	NewWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	NewWeapon->SetOwner(this);
	
	for(int i =0; i<MaxWeaponSlots; i++)
	{
		if (!WeaponInventory.IsValidIndex(i) || IsValid(WeaponInventory[i]))
		{
			continue;
		}
		
		WeaponInventory[i] = NewWeapon;
		CurrentWeapon = NewWeapon;
	
		//빈 공간이 있어 무기가 슬롯에 설정되면 함수가 여기에서 종료
		return;
	}

	for (int i =0; i<MaxWeaponSlots; i++)
	{
		if (!WeaponInventory.IsValidIndex(i) || !IsValid(WeaponInventory[i]))
		{
			continue;
		}
		
		if (WeaponInventory[i] == CurrentWeapon)
		{

			int32 const PrevWeaponId = CurrentWeapon->GetWeaponId();
			//UClass* SpawnClass = StaticLoadClass(AItemRoot::StaticClass(), nullptr, *FString::Printf(TEXT("/Game/FirstPersonCPP/Blueprints/Weapon%dRoot.Weapon%dRoot_C"), PrevWeaponId, PrevWeaponId));
			//GetWorld()->SpawnActor<AItemRoot>(SpawnClass, GetActorLocation()+GetActorForwardVector()*250, FRotator::ZeroRotator);

			auto ItemRootClassFinder = GetGameInstance<UDemoFPSGameInstance>()->GetItemRootClass();
			if(ItemRootClassFinder.Contains(static_cast<EWeaponRootClassKey>(PrevWeaponId)))
			{
				UClass* SpawnClass = ItemRootClassFinder[static_cast<EWeaponRootClassKey>(PrevWeaponId)];
				if (SpawnClass != nullptr)
				{
					AItemRoot* SpawnedRoot = GetWorld()->SpawnActor<AItemRoot>(SpawnClass, GetActorLocation()+GetActorForwardVector()*250, FRotator::ZeroRotator);
					if (!IsValid(SpawnedRoot))
					{
						return;
					}
				}
			}
			
			CurrentWeapon->Destroy();
			CurrentWeapon = NewWeapon;
			WeaponInventory[i] = NewWeapon;
		}
	}
}
/*
int32 ADemoFPSCharacter::GetCurrentAmmoCount() const
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

void ADemoFPSCharacter::AddCurrentAmmoCount(int32 AmmoId)
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
*/
void ADemoFPSCharacter::ReloadWeapon()
{
	if (!IsValid(CurrentWeapon) || AmmunitionBag == nullptr)
	{
		return;
	}
	
	int32 NewAmmo = CurrentWeapon->NeedAmmo();
	int32 const AmmoId = CurrentWeapon->GetAmmoId();
	int32 const CurrentWeaponAmmoCount = AmmunitionBag->GetAmmoXCount(AmmoId);

	if (NewAmmo > CurrentWeaponAmmoCount)
	{
		NewAmmo = CurrentWeaponAmmoCount;
		AmmunitionBag->DeductAmmoXCount(AmmoId, NewAmmo);
	}

	CurrentWeapon->Reload(NewAmmo);
}

void ADemoFPSCharacter::EquipSlot1()
{
	if (!WeaponInventory.IsValidIndex(0))
	{
		return;
	}
	
	if (IsValid(WeaponInventory[0]) && CurrentWeapon != WeaponInventory[0])
	{
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon = WeaponInventory[0];
		CurrentWeapon->SetActorHiddenInGame(false);
	}
}

void ADemoFPSCharacter::EquipSlot2()
{
	if (!WeaponInventory.IsValidIndex(1))
	{
		return;
	}
	
	if (IsValid(WeaponInventory[1]) && CurrentWeapon != WeaponInventory[1])
	{
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon = WeaponInventory[1];
		CurrentWeapon->SetActorHiddenInGame(false);
	}
}

void ADemoFPSCharacter::EquipSlot3()
{
	if (!WeaponInventory.IsValidIndex(2))
	{
		return;
	}
	
	if (IsValid(WeaponInventory[2]) && CurrentWeapon != WeaponInventory[2])
	{
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon = WeaponInventory[2];
		CurrentWeapon->SetActorHiddenInGame(false);
	}
	
}

void ADemoFPSCharacter::RestartGame()
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	ADemoFPSHUD* const MyHUD = PlayerController->GetHUD<ADemoFPSHUD>();
	if (!IsValid(MyHUD))
	{
		return;
	}
	
	if (MyHUD->GetGameOver() == true)
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("FirstPersonExampleMap"));
	}
}

void ADemoFPSCharacter::AddDamage(int32 NewDamage)
{
	if (NewDamage < 0)
	{
		return;
	}
	
	CurrentHP = FMath::Clamp<int32>(CurrentHP - NewDamage, 0 , 100);
	
	UE_LOG(LogTemp, Warning, TEXT("%s got %d damage"), *GetName(), NewDamage)
	if (CurrentHP <= 0)
	{
		Destroy();
	}
}

void ADemoFPSCharacter::OnFireWeapon()
{
	if (!IsValid(CurrentWeapon))
	{
		return;
	}
	
	if (CurrentWeapon->FireGun() == true)
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

void ADemoFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ADemoFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ADemoFPSCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADemoFPSCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ADemoFPSCharacter::AimOn()
{
	if (FirstPersonCameraComponent == nullptr || AimCamera == nullptr)
	{
		return;
	}

	if (!IsValid(CurrentWeapon))
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("AimOn"));
	FirstPersonCameraComponent->SetActive(false);
	AimCamera->SetActive(true);
	AimCamera->AttachToComponent(CurrentWeapon->GetMuzzle(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
}

void ADemoFPSCharacter::AimOff()
{
	if (FirstPersonCameraComponent == nullptr || AimCamera == nullptr)
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("AimOff"));
	FirstPersonCameraComponent->SetActive(true);
	AimCamera->AttachToComponent(FirstPersonCameraComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
	AimCamera->SetRelativeLocation(GunOffset);
	AimCamera->SetActive(false);
}

void ADemoFPSCharacter::OnBombKeyPressed()
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	if (AmmunitionBag == nullptr)
	{
		return;
	}
	if (BombClass == nullptr) 
	{
		return;
	}
	if (AmmunitionBag->GetCurrentGrenadeCount() <= 0)
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
	AmmunitionBag->DeductCurrentGrenadeCount(1);
	UE_LOG(LogTemp, Warning, TEXT("Bombing Engaged"));
}

void ADemoFPSCharacter::AddCurrentHP(int32 NewHP)
{
	if (NewHP <= 0)
	{
		return;
	}
	
	CurrentHP = FMath::Clamp<int32>(CurrentHP+NewHP, 0, 100);
}


