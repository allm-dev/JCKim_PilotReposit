// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoFPSProjectile.h"

#include "DemoFPSCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ADemoFPSProjectile::ADemoFPSProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ADemoFPSProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	Damage =30;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ADemoFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor == GetOwner())
	{
		return;
	}
	
	ADemoFPSCharacter* DownCastedActor = Cast<ADemoFPSCharacter>(OtherActor);
	if (DownCastedActor != nullptr)
	{
		DownCastedActor->AddDamage(Damage);

		if (!IsValid(DownCastedActor))
		{
			/*
			ADemoFPSCharacter* Killer =  Cast<ADemoFPSCharacter>(GetOwner());
			if(IsValid(Killer))
			{
				UE_LOG(LogTemp, Warning, TEXT("GunShot Kill %s"), *OtherCharacterName);
				Killer->AddKillScore(1);
			}
			*/
			OnGSWKill.Broadcast(1);
		}
		
		Destroy();
	}
	
	else if (OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}