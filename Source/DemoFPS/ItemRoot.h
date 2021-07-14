// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "ItemRoot.generated.h"

UENUM()
enum EItemType
{
	Weapon0 =0,
	Weapon1,
	Weapon2,
	Ammo0,
	Ammo1,
	Ammo2,
	Grenade,
	HealPackSmall,
	HealPackHuge
};

UCLASS()
class DEMOFPS_API AItemRoot : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category=Collier, Meta=(AllowPrivateAccess = true))
	USphereComponent* SphereComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh, Meta =(AllowPrivateAcess = true))
	UStaticMeshComponent*  MeshCompST;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh, Meta =(AllowPrivateAcess = true))
	USkeletalMeshComponent*  MeshCompSK;

	UPROPERTY(EditDefaultsOnly, Category=ItemSort, Meta =(AllowPrivateAcess = true))
	TEnumAsByte<EItemType> ItemType;
	
public:	
	// Sets default values for this actor's properties
	AItemRoot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

};


