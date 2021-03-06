// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoFPSCharacter.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


class ADemoFPSProjectile;

UCLASS()
class DEMOFPS_API AWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh, Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Owner, Meta = (AllowPrivateAccess = true), Replicated)
	ADemoFPSCharacter* GunOwner;

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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	USceneComponent* GetMuzzle() const {return Muzzle;}
	
	UFUNCTION()
	bool FireGun();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireGun(const FVector& SpawnLocation, const FRotator& SpawnRotation);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastFireGun();

	UFUNCTION()
	void SpawnBullet(const FVector& SpawnLocation, const FRotator& SpawnRotation);

	UFUNCTION()
	bool Reload(int32 NewAmmo);

	UFUNCTION()
	int32 GetCurrentAmmo() const
	{
		return CurrentAmmo;
	}

	UFUNCTION()
	int32 GetMaxAmmo() const
	{
		return MaxAmmo;
	}


	UFUNCTION()
	int32 NeedAmmo() const
	{
		return MaxAmmo - CurrentAmmo;
	}

	UFUNCTION()
	int32 GetAmmoId() const
	{
		return AmmoId;
	}

	UFUNCTION()
	FString GetWeaponName() const
	{
		return WeaponName;
	}

	UFUNCTION()
	int32 GetWeaponId() const
	{
		return WeaponId;
	}

	UFUNCTION(BlueprintCallable)
	USkeletalMesh* GetGunSkeletalMesh() const
	{
		return Mesh->SkeletalMesh;
	}

	UFUNCTION(BlueprintCallable)
	UMaterialInterface* GetGunMaterialInterface() const
	{
		return Mesh->GetMaterial(0);
	}
	
	
};

