// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DemoFPSCharacter.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "ItemRoot.generated.h"

UENUM(Blueprintable)
enum EItemType
{
	Weapon0 = 0,
	Weapon1,
	Weapon2,
	Ammo0,
	Ammo1,
	Ammo2,
	Grenade,
	HealPackSmall,
	HealPackHuge,
	End
};

UCLASS()
class DEMOFPS_API AItemRoot : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleDefaultsOnly, Category=Collier)
	USphereComponent* SphereComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	UStaticMeshComponent*  MeshCompST;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent*  MeshCompSK;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemSort)
	TEnumAsByte<EItemType> ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemPath)
	TMap<TEnumAsByte<EItemType>, TSubclassOf<AWeapon>> ItemPathFinder;
	
	AItemRoot();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintCallable)
	void SetItemType(EItemType NewItemType) {ItemType = NewItemType;}

	AWeapon* GenerateWeaponRoot(EItemType NewItemType);

	void GenerateRandAmmoRoot(int32 WeaponAmmoId, int32 Min, int32 Max, ADemoFPSCharacter* Receiver);

	void GenerateRandGrenadeRoot(int32 Min, int32 Max, ADemoFPSCharacter* Receiver);
};







