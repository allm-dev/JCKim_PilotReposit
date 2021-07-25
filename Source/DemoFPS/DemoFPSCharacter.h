// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DemoFPSGameInstance.h"
#include "GameFramework/Character.h"
#include "DemoFPSCharacter.generated.h"

class AWeapon;
class ADemoFPSPlayerState;
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

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh3P;

	UPROPERTY(EditAnywhere, Category=Weapon, Replicated)
	TArray<AWeapon*> WeaponInventory;

	UPROPERTY(VisibleAnywhere, Category=Weapon)
	int32 MaxWeaponSlots;

	UPROPERTY(EditAnywhere, Category = Weapon, Replicated)
	AWeapon* CurrentWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category = GunMesh3P, Replicated)
	USkeletalMeshComponent* GunMesh3P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* AimCamera;

	UPROPERTY(VisibleDefaultsOnly, Category = Ammunition)
	UAmmunitionComp* AmmunitionBag;

	UPROPERTY(VisibleDefaultsOnly, Category = PalyerState)
	ADemoFPSPlayerState* MyPlayerState;
	
protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GamePlay)
	UAnimMontage* FireAnim3P;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=GamePlay)
	UAnimMontage* HitAnim3P;

	UFUNCTION()
	AWeapon* GetCurrentWeapon() const {return CurrentWeapon;}

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetWeaponInSlot(EWeaponClassKey WeaponClassKey);
	
	UFUNCTION()
	void SetWeaponInSlot(EWeaponClassKey WeaponClassKey);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastVisualizeCurrentWeapon();

	FTimerHandle MultiCastVisualizeWeaponInSlotDelay; 

	UFUNCTION(BlueprintCallable)
	ADemoFPSPlayerState* GetMyPlayerState() const
	{
		return MyPlayerState;	
	}

	UFUNCTION(BlueprintCallable)
	void SetMyPlayerState(ADemoFPSPlayerState* NewPlayerState)
	{
		MyPlayerState = NewPlayerState;
	}

	UFUNCTION(BlueprintCallable)
	void OnFireWeapon();

	UFUNCTION()
	void MoveForward(float Val);

	UFUNCTION()
	void MoveRight(float Val);

	UFUNCTION(BlueprintCallable)
	void ReloadWeapon();

	UFUNCTION(Server, Reliable)
	void ServerEquipSlotX(uint8 SlotNum);
	
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

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnBomb(const FVector& SpawnLocation, const FRotator& SpawnRotation);

	UFUNCTION(Server, Reliable)
	void ServerPlayFireAnim();

	UFUNCTION(Client, Reliable)
	void PlayHitAnim3P();

	UFUNCTION(Server, Reliable)
	void ServerPlayHitAnim3P();

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastPlayHitAnim3P();

	

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastPlayFireAnim();

	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable)
	UAmmunitionComp* GetAmmunitionComp() const {return AmmunitionBag;}
};



