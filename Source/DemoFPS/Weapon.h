// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


class ADemoFPSProjectile;

UCLASS()
class DEMOFPS_API AWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh, Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh, Meta =(AllowPrivateAccess = true))
	USceneComponent* Muzzle;

	UPROPERTY(EditDefaultsOnly, Category=Ammunition, Meta = (AllowPrivateAccess = true))
	TSubclassOf<ADemoFPSProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category=Ammunition, Meta = (AllowPrivateAccess = true))
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, Category=Ammunition, Meta =(AllowPrivateAccess = true))
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category=Ammunition, Meta = (AllowPrivateAccess=true))
	int32 AmmoId;

	UPROPERTY(EditDefaultsOnly, Category=Name, Meta = (AllowPrivateAcces=true))
	FString WeaponName;

	UPROPERTY(EditDefaultsOnly, Category = Name, Meta = (AllowPrivateAcces =true))
	int32 WeaponId;
	
public:	
	AWeapon();

	UFUNCTION()
	USceneComponent* GetMuzzle() const {return Muzzle;}
	
	UFUNCTION()
	bool FireGun();

	UFUNCTION()
	bool Reload(int32 NewAmmo);

	UFUNCTION()
	int32 GetCurrentAmmo() const {return CurrentAmmo;}

	UFUNCTION()
	int32 GetMaxAmmo() const {return MaxAmmo;}


	UFUNCTION()
	int32 NeedAmmo() const {return MaxAmmo - CurrentAmmo;}

	UFUNCTION()
	int32 GetAmmoId() const {return AmmoId;}

	UFUNCTION()
	FString GetWeaponName() const {return WeaponName;}

	UFUNCTION()
	int32 GetWeaponId() const {return WeaponId;}
	
	
};
