// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DemoFPSGameInstance.generated.h"

UENUM(Blueprintable)
enum EWeaponRootClassKey
{
	Weapon1Root = 0,
	Weapon2Root, 
	Weapon3Root,
	WeaponRootClassKeyEnd
};

UENUM(Blueprintable)
enum EWeaponClassKey
{
	WeaponClass1 = 0,
	WeaponClass2,
	WeaponClass3,
	WeaponClassEnd
};

class AItemRoot;
class AWeapon;

UCLASS()
class DEMOFPS_API UDemoFPSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UDemoFPSGameInstance();

	UPROPERTY(EditDefaultsOnly, Category=WeaponRootClass)
	TMap<TEnumAsByte<EWeaponRootClassKey>, TSubclassOf<AItemRoot>> WeaponRootClass;

	UPROPERTY(EditDefaultsOnly, Category=WeaponClass)
	TMap<TEnumAsByte<EWeaponClassKey>, TSubclassOf<AWeapon>> WeaponClass;

	UFUNCTION(BlueprintCallable)
	TMap<TEnumAsByte<EWeaponRootClassKey>, TSubclassOf<AItemRoot>> GetWeaponRootClass() const {return WeaponRootClass;}

	UFUNCTION(BlueprintCallable)
	TMap<TEnumAsByte<EWeaponClassKey>, TSubclassOf<AWeapon>> GetWeaponClass() const {return WeaponClass;}
};
