// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "Grenade.generated.h"

UCLASS()
class DEMOFPS_API AGrenade : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = Collider)
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleDefaultsOnly, Category = ExplosiveRange)
	USphereComponent* ExplosionSphere;

	UPROPERTY(VisibleDefaultsOnly, Category=Effect)
	UParticleSystemComponent* DefaultEffect;

	UPROPERTY(VisibleDefaultsOnly, Category=Effect)
	UParticleSystemComponent* ExplosionEffect;

	UPROPERTY(VisibleDefaultsOnly, Category=MovementComp)
	UProjectileMovementComponent* MovementComp;
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

	UFUNCTION()
	void OnExplosion( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LifeSpanExpired() override;
	


};
