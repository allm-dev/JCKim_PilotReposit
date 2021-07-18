// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AmmunitionComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEMOFPS_API UAmmunitionComp : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = AmmoCount)
	int32 Ammo0Count;

	UPROPERTY(EditAnywhere, Category = AmmoCount)
	int32 Ammo1Count;

	UPROPERTY(EditAnywhere, Category = AmmoCount)
	int32 Ammo2Count;

	UPROPERTY(EditAnywhere, Category = AmmoCount)
	int32 GrenadeCount;

public:	
	// Sets default values for this component's properties
	UAmmunitionComp();

	UFUNCTION()
	int32 GetAmmoXCount(int32 AmmoId) const;

	UFUNCTION()
	void DeductAmmoXCount(int32 AmmoId, int32 Amount);

	UFUNCTION()
	void AddAmmoXCount(int32 AmmoId, int32 Amount);
	
	UFUNCTION()
	int32 GetCurrentGrenadeCount() const {return GrenadeCount;}

	UFUNCTION()
	void AddCurrentGrenadeCount(int32 Amount);

	UFUNCTION()
	void DeductCurrentGrenadeCount(int32 Amount);

};
