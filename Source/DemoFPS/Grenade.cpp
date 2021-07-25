// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"
#include "DemoFPSCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGrenade::AGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->InitCapsuleSize(15.0f, 25.0f);
	Capsule->SetCollisionProfileName("Projectile");

	Mesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Capsule(TEXT("/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule"));
	check(SM_Capsule.Succeeded());
	Mesh->SetStaticMesh(SM_Capsule.Object);
	static ConstructorHelpers::FObjectFinder<UMaterial> Mat_Clay(TEXT("/Game/StarterContent/Materials/M_Brick_Clay_New.M_Brick_Clay_New"));
	check(Mat_Clay.Succeeded());
	Mesh->SetMaterial(0, Mat_Clay.Object);
	Mesh->SetRelativeLocation(FVector(0,0,-25));
	Mesh->SetRelativeScale3D(FVector(0.5f,0.5f,0.5f));

	ExplosionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionSphere"));
	ExplosionSphere->InitSphereRadius(0.0f);
	ExplosionSphere->SetCollisionProfileName("OverlapOnlyPawn");
	ExplosionSphere->OnComponentBeginOverlap.AddDynamic(this, &AGrenade::OnExplosion);
	ExplosionSphere->SetRelativeLocation(FVector(0,0,0));
	ExplosionSphere->bMultiBodyOverlap=true;

	DefaultEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PS_Default"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_Default(TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire"));
	check(PS_Default.Succeeded());
	DefaultEffect->SetTemplate(PS_Default.Object);
	DefaultEffect->SetRelativeScale3D(FVector(0.5f,0.5f,0.5f));
	DefaultEffect->SetRelativeLocation(FVector(0.0f,0.0f, 25.0f));
	
	ExplosionEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PS_Explosion"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_Explosion(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	check(PS_Explosion.Succeeded());
	ExplosionEffect->SetTemplate(PS_Explosion.Object);
	ExplosionEffect->SetAutoActivate(false);
	ExplosionSphere->SetRelativeLocation(FVector(0.0f,0.0f,0.0f));
	ExplosionEffect->SetRelativeScale3D(FVector(2.0f,2.0f,2.0f));
	ExplosionEffect->OnComponentActivated.AddDynamic(this, &AGrenade::OnExplosionStart);
	ExplosionEffect->OnSystemFinished.AddDynamic(this, &AGrenade::OnExplosionEnd);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->SetUpdatedComponent(Capsule);
	MovementComp->InitialSpeed = 2000.0f;
	MovementComp->MaxSpeed = 2000.0f;
	MovementComp->ProjectileGravityScale = 1.0f;
	MovementComp->bRotationFollowsVelocity=true;
	MovementComp->bShouldBounce=true;
	MovementComp->Bounciness=0.25f;
	MovementComp->BounceVelocityStopSimulatingThreshold = 250.0f;

	RootComponent = Capsule;
	Mesh->SetupAttachment(RootComponent);
	ExplosionSphere->SetupAttachment(RootComponent);
	DefaultEffect->SetupAttachment(RootComponent);
	ExplosionEffect->SetupAttachment(RootComponent);

	ExplosionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InitialLifeSpan = 3.0f;
	
	bReplicates = true;
}


void AGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AGrenade::OnExplosion(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority())
	{
		return;
	}
	
	ServerOnExplosion(OtherActor);
}


void AGrenade::ServerOnExplosion_Implementation(AActor* OtherActor)
{
	ADemoFPSCharacter* DownCastedActor = Cast<ADemoFPSCharacter>(OtherActor);
	if (DownCastedActor == nullptr)
	{
		return;
	}
	
	UCapsuleComponent* ActorCapsuleComp = DownCastedActor->GetCapsuleComponent();
	if (ActorCapsuleComp == nullptr)
	{
		return;
	}
	
	ActorCapsuleComp->SetSimulatePhysics(true);
	FVector BlowDir = DownCastedActor->GetTargetLocation() - GetActorLocation();
	BlowDir.Normalize();
	DownCastedActor->GetCapsuleComponent()->AddImpulse(BlowDir *1000.0f, NAME_None, true);
	
	DownCastedActor->SetLifeSpan(0.5f);

	if (DownCastedActor == GetOwner<ADemoFPSCharacter>())
	{
		return;
	}
	
	if (OnGrenadeKill.ExecuteIfBound(1) == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grenade Kill Report Successful"));
	}

}


void AGrenade::LifeSpanExpired()
{
	if (!HasAuthority())
	{
		return;
	}
	
	ServerLifeSpanExpired();
}


void AGrenade::ServerLifeSpanExpired_Implementation()
{
	MultiCastLifeSpanExpired();
}

void AGrenade::MultiCastLifeSpanExpired_Implementation()
{
	DefaultEffect->Deactivate();
	ExplosionEffect->Activate();
	Mesh->SetVisibility(false);
}

void AGrenade::OnExplosionStart(UActorComponent* Component, bool bReset)
{
	if (!HasAuthority())
	{
		return;
	}
	
	ServerOnExplosionStart();
}

void AGrenade::ServerOnExplosionStart_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Server grenade explosion starts"));
	ExplosionSphere->SetSphereRadius(300.0f);
	ExplosionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	MultiCastOnExplosionStart();
}

void AGrenade::MultiCastOnExplosionStart_Implementation()
{
	GetWorldTimerManager().SetTimer(CustomTimerHandle, this, &AGrenade::CustomTimerDelayedDestroy, 0.2f);
}


void AGrenade::CustomTimerDelayedDestroy()
{
	GetWorldTimerManager().ClearTimer(CustomTimerHandle);
	UE_LOG(LogTemp, Warning, TEXT("Destroying Grenade Explosion Component"));
	ExplosionSphere->DestroyComponent();	
}


void AGrenade::OnExplosionEnd(UParticleSystemComponent* PS)
{
	ServerOnExplosionEnd();
}

void AGrenade::ServerOnExplosionEnd_Implementation()
{
	MultiCastOnExplosionEnd();
}

void AGrenade::MultiCastOnExplosionEnd_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Grenade has destroyed"));
	Destroy();
}


