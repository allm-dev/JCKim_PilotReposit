// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
public:	
	AWeapon();

	UFUNCTION()
	bool FireGun();

	UFUNCTION()
	bool Reload(int32 NewAmmo);

	inline int32 NeedAmmo() {return MaxAmmo - CurrentAmmo;};

	inline int32 GetAmmoId() {return AmmoId;}
	
protected:
	virtual void BeginPlay() override;

	

};
