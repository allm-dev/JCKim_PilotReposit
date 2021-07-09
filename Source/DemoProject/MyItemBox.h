// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DemoProject.h"
#include "GameFramework/Actor.h"
#include "MyItemBox.generated.h"

UCLASS()
class DEMOPROJECT_API AMyItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyItemBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
public:	

	//collision component
	UPROPERTY(VisibleAnywhere, Category=Box)
	UBoxComponent* Trigger;

	//mesh component
	UPROPERTY(VisibleAnywhere, Category=Box)
	UStaticMeshComponent* Box;

	UPROPERTY(EditInstanceOnly, Category=Box)
	TSubclassOf<class AMyWeapon> WeaponItemClass;

	UPROPERTY(VisibleAnywhere, Category=Effect)
	UParticleSystemComponent* Effect;
	
private:
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* ParticleSys);
};
