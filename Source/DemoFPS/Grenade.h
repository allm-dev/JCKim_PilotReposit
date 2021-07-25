// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Grenade.generated.h"

DECLARE_DELEGATE_OneParam(OnGrenadeKillDelegate, uint8)

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

	UPROPERTY(VisibleDefaultsOnly, category = Mesh)
	UStaticMeshComponent* Mesh;
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

	UFUNCTION()
	void OnExplosion(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnExplosionStart(UActorComponent* Component, bool bReset);
	
	UFUNCTION()
	void OnExplosionEnd( UParticleSystemComponent* PS);

	OnGrenadeKillDelegate OnGrenadeKill;;

	FTimerHandle CustomTimerHandle;

	UFUNCTION(BlueprintCallable)
	void CustomTimerDelayedDestroy();
protected:

	virtual void LifeSpanExpired() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	UFUNCTION(Server, Reliable)
	void ServerLifeSpanExpired();
	
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastLifeSpanExpired();

	UFUNCTION(Server, Reliable)
	void ServerOnExplosionStart();

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastOnExplosionStart();
	
	UFUNCTION(Server, Reliable)
	void ServerOnExplosion(AActor* OtherActor);

	UFUNCTION(Server, Reliable)
	void ServerOnExplosionEnd();
	
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastOnExplosionEnd();

};


