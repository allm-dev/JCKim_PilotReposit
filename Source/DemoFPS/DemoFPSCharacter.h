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
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ADemoFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditAnywhere, Category=Weapon,meta = (AllowPrivateAccess = "true"))
	TArray<AWeapon*> WeaponInventory;

	UPROPERTY(VisibleAnywhere, Category=Weapon,meta = (AllowPrivateAccess = "true"))
	int32 MaxWeaponSlots;

	UPROPERTY(EditAnywhere, Category = Weapon,meta = (AllowPrivateAccess = "true"))
	AWeapon* CurrentWeapon;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* AimCamera;

	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	TSubclassOf<AWeapon> DefaultGunClass;

	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	TSubclassOf<AWeapon> DefaultGunClass2;

	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	TSubclassOf<AWeapon> DefaultGunClass3;

	UPROPERTY(EditAnywhere, Category = AmmoCount, meta = (AllowPrivateAccess = true))
	int32 Ammo0Count;

	UPROPERTY(EditAnywhere, Category = AmmoCount, meta = (AllowPrivateAccess = true))
	int32 Ammo1Count;

	UPROPERTY(EditAnywhere, Category = AmmoCount, meta = (AllowPrivateAccess = true))
	int32 Ammo2Count;

	UPROPERTY(EditAnywhere, Category = AmmoCount, meta = (AllowPrivateAccess = true))
	int32 GrenadeCount;

	UPROPERTY(EditAnywhere, Category = KillCount, meta = (AllowPrivateAccess = true))
	int32 KillScore;

	UPROPERTY(EditAnywhere, Category = KillCount, meta = (AllowPrivateAccess = true))
	int32 HP;
public:
	ADemoFPSCharacter();

protected:
	virtual void BeginPlay();
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AGrenade> BombClass;
	
	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	AWeapon* GetCurrentWeapon() const {return CurrentWeapon;}
	void SetWeapon(AWeapon* NewWeapon);

	int32 GetAmmoCount() const;

	void SetAmmoCountUp(int32 AmmoId);
	
	int32 GetGrenadeCount() const {return GrenadeCount;}
	void SetGrenadeCountUp() {GrenadeCount += FMath::RandRange(1,5);}
	int32 GetKillScore() const {return KillScore;}

	int32 GetHP() const {return HP;}
	void SetHPUp(int32 NewHP) {HP = FMath::Clamp<int32>(HP+NewHP, 0, 100);}
	
	void SetDamage(int32 NewDamage);
	void SetKillScoreUp(int32 NewKillScore) {KillScore += NewKillScore; UE_LOG(LogTemp, Warning, TEXT("Kill Score Up"));}


protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	void Reload();

	void EquipSlot1();
	void EquipSlot2();
	void EquipSlot3();

	void RestartGame();

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void AimOn();
	void AimOff();

	void OnBomb();

private:

	FVector CameraDefaultPos = FVector::ZeroVector;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface



public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

