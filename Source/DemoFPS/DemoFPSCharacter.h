// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DemoFPSCharacter.generated.h"

class AWeapon;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAmmunitionComp;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ADemoFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditAnywhere, Category=Weapon)
	TArray<AWeapon*> WeaponInventory;

	UPROPERTY(VisibleAnywhere, Category=Weapon)
	int32 MaxWeaponSlots;

	UPROPERTY(EditAnywhere, Category = Weapon)
	AWeapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* AimCamera;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AWeapon> DefaultGunClass;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AWeapon> DefaultGunClass2;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AWeapon> DefaultGunClass3;

	UPROPERTY(VisibleDefaultsOnly, Category = Ammunition)
	UAmmunitionComp* AmmunitionBag;

	/*
	UPROPERTY(EditAnywhere, Category = AmmoCount)
	int32 Ammo0Count;

	UPROPERTY(EditAnywhere, Category = AmmoCount)
	int32 Ammo1Count;

	UPROPERTY(EditAnywhere, Category = AmmoCount)
	int32 Ammo2Count;

	UPROPERTY(EditAnywhere, Category = AmmoCount)
	int32 GrenadeCount;
	*/
	
	UPROPERTY(EditAnywhere, Category = KillCount)
	int32 KillScore;

	UPROPERTY(EditAnywhere, Category = HealthPoint)
	int32 CurrentHP;

	UPROPERTY(EditDefaultsOnly, Category = HelathPoint)
	int32 MaxHP;

	
protected:
	
	UFUNCTION()
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	ADemoFPSCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AGrenade> BombClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	UFUNCTION()
	AWeapon* GetCurrentWeapon() const {return CurrentWeapon;}

	UFUNCTION()
	void SetWeaponInSlot(AWeapon* NewWeapon);

	/*
	UFUNCTION()
	int32 GetCurrentAmmoCount() const;
	
	UFUNCTION()
	void AddCurrentAmmoCount(int32 AmmoId);
	*/

	/*
	UFUNCTION()
	int32 GetGrenadeCount() const {return GrenadeCount;}

	UFUNCTION()
	void SetGrenadeCountUp() {GrenadeCount += FMath::RandRange(1,5);}
	*/

	UFUNCTION()
	int32 GetKillScore() const {return KillScore;}

	UFUNCTION()
	int32 GetCurrentHP() const {return CurrentHP;}

	UFUNCTION()
	void AddCurrentHP(int32 NewHP);
	
	UFUNCTION()
	void AddDamage(int32 NewDamage);

	UFUNCTION()
	void AddKillScore(int32 NewKillScore) {KillScore += NewKillScore; UE_LOG(LogTemp, Warning, TEXT("Kill Score Up"));}

protected:
	
	/** Fires a projectile. */
	UFUNCTION(BlueprintCallable)
	void OnFireWeapon();

	/** Handles moving forward/backward */
	UFUNCTION()
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	UFUNCTION()
	void MoveRight(float Val);

	UFUNCTION(BlueprintCallable)
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable)
	void EquipSlot1();

	UFUNCTION(BlueprintCallable)
	void EquipSlot2();

	UFUNCTION(BlueprintCallable)
	void EquipSlot3();

	UFUNCTION(BlueprintCallable)
	void RestartGame();

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	void AimOn();

	void AimOff();

	UFUNCTION(BlueprintCallable)
	void OnBombKeyPressed();
	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:

	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable)
	UAmmunitionComp* GetAmmunitionComp() const {return AmmunitionBag;}
};

