// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoFPSCharacter.h"
#include "DemoFPSGameInstance.h"
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
#include "DemoFPSGameState.h"
#include "DemoFPSPlayerState.h"
#include "Net/UnrealNetwork.h"

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

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	Mesh3P->bCastDynamicShadow = true;
	Mesh3P->CastShadow = true;
	Mesh3P->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	Mesh3P->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));

	GunMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh3P"));
	GunMesh3P->SetOwnerNoSee(true);
	GunMesh3P->SetupAttachment(Mesh3P);
	GunMesh3P->bCastDynamicShadow = true;
	GunMesh3P->CastShadow = true;
	GunMesh3P->SetSkeletalMesh(nullptr);

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

	AmmunitionBag = CreateDefaultSubobject<UAmmunitionComp>(TEXT("AmmunitionBag"));

	bReplicates = true;
}

void ADemoFPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GunMesh3P->AttachToComponent(Mesh3P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("HandGrip"));
	BombClass = AGrenade::StaticClass();
}

void ADemoFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	SetWeaponInSlot(EWeaponClassKey::WeaponClass1);
	SetWeaponInSlot(EWeaponClassKey::WeaponClass2);
	SetWeaponInSlot(EWeaponClassKey::WeaponClass3);
    
	EquipSlot1();
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

void ADemoFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADemoFPSCharacter, WeaponInventory);
	DOREPLIFETIME(ADemoFPSCharacter, CurrentWeapon);
	DOREPLIFETIME(ADemoFPSCharacter, GunMesh3P);
}

bool ADemoFPSCharacter::ServerSetWeaponInSlot_Validate(EWeaponClassKey WeaponClassKey)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}
	
	UDemoFPSGameInstance* GameInstance = World->GetGameInstance<UDemoFPSGameInstance>();
	if (GameInstance ==  nullptr)
	{
		return false;
	}

	TMap<TEnumAsByte<EWeaponClassKey>, TSubclassOf<AWeapon>> const WeaponClassFinder = GameInstance->GetWeaponClass();
	if (!WeaponClassFinder.Contains(WeaponClassKey))
	{
		return false;
	}

	if (WeaponClassFinder[WeaponClassKey] == nullptr)
	{
		return false;
	}

	return true;
}


void ADemoFPSCharacter::ServerSetWeaponInSlot_Implementation(EWeaponClassKey WeaponClassKey)
{
	TMap<TEnumAsByte<EWeaponClassKey>, TSubclassOf<AWeapon>> const WeaponClassFinder = GetGameInstance<UDemoFPSGameInstance>()->GetWeaponClass();

	UClass* const NewWeaponClass = WeaponClassFinder[WeaponClassKey];
	AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(NewWeaponClass);
	
	if (!IsValid(NewWeapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Spawn Weapon on server"))
		return;
	}

	//인벤토리 내 빈공간 찾기
	for(int i =0; i<MaxWeaponSlots; i++)
	{
		if (!WeaponInventory.IsValidIndex(i) || IsValid(WeaponInventory[i]))
		{
			continue;
		}

		WeaponInventory[i] = NewWeapon;
		CurrentWeapon = NewWeapon;
		CurrentWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
		CurrentWeapon->SetOwner(this);
		GetWorldTimerManager().SetTimer(MultiCastVisualizeWeaponInSlotDelay, this, &ADemoFPSCharacter::MultiCastVisualizeCurrentWeapon, 0.2f, false);

		return;
	}

	//인벤토리에 빈공간이 없음
	for (int i =0; i<MaxWeaponSlots; i++)
	{
		if (!WeaponInventory.IsValidIndex(i) || !IsValid(WeaponInventory[i]))
		{
			continue;
		}
		
		if (WeaponInventory[i] == CurrentWeapon)
		{

			int32 const PrevWeaponId = CurrentWeapon->GetWeaponId();

			TMap<TEnumAsByte<EWeaponRootClassKey>, TSubclassOf<AItemRoot>> ItemRootClassFinder = GetGameInstance<UDemoFPSGameInstance>()->GetWeaponRootClass();
			if (!ItemRootClassFinder.Contains(static_cast<EWeaponRootClassKey>(PrevWeaponId)))
			{
				continue;
			}
			
			UClass* NewWeaponRootClass = ItemRootClassFinder[static_cast<EWeaponRootClassKey>(PrevWeaponId)];
			if (NewWeaponRootClass == nullptr)
			{
				continue;
			}
			
			AItemRoot* NewWeaponRoot = GetWorld()->SpawnActor<AItemRoot>(NewWeaponRootClass, GetActorLocation()+GetActorForwardVector()*250, FRotator::ZeroRotator);
			if (!IsValid(NewWeaponRoot))
			{
				continue;
			}
			
			UE_LOG(LogTemp, Warning, TEXT("Spawning new itemRoot"));
			CurrentWeapon->Destroy();
			CurrentWeapon = NewWeapon;
			WeaponInventory[i] = NewWeapon;
			CurrentWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
			CurrentWeapon->SetOwner(this);
			GetWorldTimerManager().SetTimer(MultiCastVisualizeWeaponInSlotDelay, this, &ADemoFPSCharacter::MultiCastVisualizeCurrentWeapon, 0.2f, false);

			
			return;
		}
	}
	
}

void ADemoFPSCharacter::SetWeaponInSlot(EWeaponClassKey WeaponClassKey)
{
	if (!HasAuthority())
	{
		return;
	}

	ServerSetWeaponInSlot(WeaponClassKey);
}

void ADemoFPSCharacter::MultiCastVisualizeCurrentWeapon_Implementation()
{
	if (!IsValid(CurrentWeapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentWeapon was not yet replicated from server due to spawning overhead"))
		return;
	}
	
	if (!IsValid(GunMesh3P))
	{
		UE_LOG(LogTemp, Error, TEXT("No %s GunMesh3P"), *GetName());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("CurrentWeapon is %s"), *CurrentWeapon->GetWeaponName());
	
	GunMesh3P->SetSkeletalMesh(CurrentWeapon->GetGunSkeletalMesh());
	GunMesh3P->SetMaterial(0, CurrentWeapon->GetGunMaterialInterface());

	for (AWeapon* Weapon : WeaponInventory)
	{
		if (!IsValid(Weapon))
		{
			continue;
		}
		
		if (Weapon == CurrentWeapon)
		{
			Weapon->SetActorHiddenInGame(false);
		}
		else
		{
			Weapon->SetActorHiddenInGame(true);
		}
	}

}

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
	}
	
	AmmunitionBag->DeductAmmoXCount(AmmoId, NewAmmo);
	CurrentWeapon->Reload(NewAmmo);
}

void ADemoFPSCharacter::ServerEquipSlotX_Implementation(uint8 SlotNum)
{
	if (!WeaponInventory.IsValidIndex(SlotNum))
	{
		return;
	}
	
	if (!IsValid(WeaponInventory[SlotNum]) || CurrentWeapon == WeaponInventory[SlotNum])
	{
		return;
	}

	CurrentWeapon = WeaponInventory[SlotNum];
	UE_LOG(LogTemp, Warning, TEXT("Server Equipped slot%d, %s"), SlotNum, *CurrentWeapon->GetWeaponName());
	GetWorldTimerManager().SetTimer(MultiCastVisualizeWeaponInSlotDelay, this, &ADemoFPSCharacter::MultiCastVisualizeCurrentWeapon, 0.2f, false);
}

void ADemoFPSCharacter::EquipSlot1()
{
	ServerEquipSlotX(0);
}

void ADemoFPSCharacter::EquipSlot2()
{
	ServerEquipSlotX(1);
}

void ADemoFPSCharacter::EquipSlot3()
{
	ServerEquipSlotX(2);
}

void ADemoFPSCharacter::RestartGame()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	
	if (!HasAuthority())
	{
		return;
	}

	ADemoFPSGameState* GameState = World->GetGameState<ADemoFPSGameState>();
	if (!IsValid(GameState))
	{
		return;
	}
	
	if (GameState->GetIsGameOver() == true)
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("FirstPersonExampleMap"));
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

		ServerPlayFireAnim();
	}
	
}

void ADemoFPSCharacter::MultiCastPlayFireAnim_Implementation()
{
	if (FireAnim3P == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No valid AnimMontage!"));
		return;
	}
	
	UAnimInstance* AnimInstance = Mesh3P->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No valid AnimInstance!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("MultiCasting FireAnim3P Montage Play"));
	AnimInstance->Montage_Play(FireAnim3P, 1.f);

}

void ADemoFPSCharacter::ServerPlayFireAnim_Implementation()
{
	MultiCastPlayFireAnim();
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
	if (AmmunitionBag == nullptr)
	{
		return;
	}
	if (BombClass == nullptr) 
	{
		UE_LOG(LogTemp, Warning, TEXT("No Valid Grenade Class on Client"));
		return;
	}
	if (AmmunitionBag->GetCurrentGrenadeCount() <= 0)
	{
		return;
	}
	
	const FRotator SpawnRotation = GetControlRotation();
	const FVector SpawnLocation =  GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

	UE_LOG(LogTemp, Warning, TEXT("Client sending grenade spawn request"));
	ServerSpawnBomb(SpawnLocation, SpawnRotation);

	AmmunitionBag->DeductCurrentGrenadeCount(1);

}

void ADemoFPSCharacter::MultiCastPlayHitAnim3P_Implementation()
{
	if (!IsValid(Mesh3P))
	{
		UE_LOG(LogTemp, Log, TEXT("%s No mesh for third person view"), *GetName());
		return;
	}
	
	UAnimInstance* AnimInstance = Mesh3P->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		UE_LOG(LogTemp, Log, TEXT("%s No AnimInstance was assigned to Mesh3P of %s"), *GetName());
		return;
	}

	AnimInstance->Montage_Play(HitAnim3P, 1.0f);
}

void ADemoFPSCharacter::ServerPlayHitAnim3P_Implementation()
{
	MultiCastPlayHitAnim3P();
}

void ADemoFPSCharacter::PlayHitAnim3P_Implementation()
{
	ServerPlayHitAnim3P();
}


void ADemoFPSCharacter::ServerSpawnBomb_Implementation(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	UWorld* const World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	auto* Grenade = World->SpawnActor<AGrenade>(BombClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	if (!IsValid(Grenade))
	{
		UE_LOG(LogTemp, Warning, TEXT("Sever Failed to spawn grenade"));
		return;
	}
	
	Grenade->SetOwner(this);
	Grenade->OnGrenadeKill.BindUObject(MyPlayerState, &ADemoFPSPlayerState::AddKillScore);
	
	UE_LOG(LogTemp, Warning, TEXT("Bombing Engaged"));
}

bool ADemoFPSCharacter::ServerSpawnBomb_Validate(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if(SpawnLocation == FVector(ForceInit) || SpawnRotation == FRotator(ForceInit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong Spawn Point Coordination Assigned"));
		return false;
	}
	
	return true;
}

void ADemoFPSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	MyPlayerState = GetPlayerState<ADemoFPSPlayerState>();
	check(IsValid(MyPlayerState))
	
}
