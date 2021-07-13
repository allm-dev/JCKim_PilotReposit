// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"

#include "DemoFPSCharacter.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

// Sets default values
AGrenade::AGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->InitCapsuleSize(2.0f, 3.0f);
	Capsule->SetCollisionProfileName("Projectile");

	ExplosionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionSphere"));
	ExplosionSphere->InitSphereRadius(100.0f);
	ExplosionSphere->SetCollisionProfileName("OverlapAll");
	ExplosionSphere->OnComponentBeginOverlap.AddDynamic(this, &AGrenade::OnExplosion);
	ExplosionSphere->SetRelativeLocation(FVector(0,0,0));
	ExplosionSphere->SetActive(false);

	DefaultEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PS_Default"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_Default(TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire"));
	check(PS_Default.Succeeded());
	DefaultEffect->SetTemplate(PS_Default.Object);
	DefaultEffect->SetRelativeScale3D(FVector(0.05f,0.05f,0.05f));
	DefaultEffect->SetRelativeLocation(FVector(0.0f,0.0f, 3.0f));
	
	ExplosionEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PS_Explosion"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_Explosion(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	check(PS_Explosion.Succeeded());
	ExplosionEffect->SetTemplate(PS_Explosion.Object);
	ExplosionEffect->SetAutoActivate(false);
	ExplosionSphere->SetRelativeLocation(FVector(0.0f,0.0f,0.0f));
	ExplosionEffect->SetRelativeScale3D(FVector(2.0f,2.0f,2.0f));

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->SetUpdatedComponent(Capsule);
	MovementComp->InitialSpeed = 2000.0f;
	MovementComp->MaxSpeed = 2500.0f;
	MovementComp->ProjectileGravityScale = 1.0f;

	RootComponent = Capsule;
	ExplosionSphere->SetupAttachment(RootComponent);
	DefaultEffect->SetupAttachment(RootComponent);
	ExplosionEffect->SetupAttachment(RootComponent);

	InitialLifeSpan = 3.0f;
}

void AGrenade::OnExplosion(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Cast<ADemoFPSCharacter>(OtherActor)!= nullptr)
	{
		OtherActor->Destroy();
	}
	Destroy();
}


// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGrenade::LifeSpanExpired()
{
	DefaultEffect->SetActive(false);
	ExplosionEffect->Activate();
	ExplosionSphere->Activate();
}


