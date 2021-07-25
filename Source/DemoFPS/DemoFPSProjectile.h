// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoFPSProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGunShotKillDelegate, uint8, ScoreAmount);

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
	uint8 Damage;


public:
	ADemoFPSProjectile();

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	UFUNCTION(BlueprintCallable)
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	//OnGSWKillDelegate OnGSWKill;
	UPROPERTY(BlueprintAssignable)
	FOnGunShotKillDelegate OnGunShotKill;
};

