// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoFPSProjectile.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(OnGSWKillDelegate, int32);

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class ADemoFPSProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(EditDefaultsOnly, Category = Damage, meta = (AllowPrivateAccess = true))
	int32 Damage;


public:
	ADemoFPSProjectile();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	UFUNCTION(BlueprintCallable)
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	OnGSWKillDelegate OnGSWKill;
};

