// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoFPSProjectile.h"
#include "DemoFPSCharacter.h"
#include "DemoFPSPlayerState.h"
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
	bReplicates = true;
}

void ADemoFPSProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SetReplicateMovement(true);
}

void ADemoFPSProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
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
		ADemoFPSPlayerState* MyPlayerState = DownCastedActor->GetMyPlayerState();
		if (!IsValid(MyPlayerState))
		{
			return;
		}
		MyPlayerState->LoseCurrentHP(Damage);
		DownCastedActor->ServerPlayHitAnim3P();

		if (!IsValid(DownCastedActor))
		{
			OnGunShotKill.Broadcast(1);
		}
		
		Destroy();
	}
	
	else if (OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}
