// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//creation
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CAMERABOOM"));

	//hierarchy
	RootComponent = Capsule;
	Mesh->SetupAttachment(Capsule);
	CameraBoom->SetupAttachment(Capsule);
	Camera->SetupAttachment(CameraBoom);

	//set detail val
	Capsule->SetCapsuleHalfHeight(88.0f);
	Capsule->SetCapsuleRadius(34.0f);
	Mesh->SetRelativeLocationAndRotation(FVector(0,0,-88), FRotator(0,-90,0));
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SetRelativeRotation(FRotator(-15,0,0));

	//skeletal mesh ref uasset data load
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>SK_Cardboard(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if(SK_Cardboard.Succeeded())
	{
		Mesh->SetSkeletalMesh(SK_Cardboard.Object);
	}

	//animation BP to Skeletal mesh
	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBP.WarriorAnimBP_C"));
	if(WARRIOR_ANIM.Succeeded())
	{
		Mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//input component axis binding
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AMyPawn::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AMyPawn::LeftRight);

}

void AMyPawn::UpDown(float NewAxisVal)
{
	//ABLOG(Warning, TEXT("%f"), NewAxisVal);
	AddMovementInput(GetActorForwardVector(), NewAxisVal);
}

void AMyPawn::LeftRight(float NewAxisVal)
{
	//ABLOG(Warning, TEXT("%f"), NewAxisVal);
	AddMovementInput(GetActorRightVector(), NewAxisVal);
}



void AMyPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AMyPawn::PossessedBy(AController* NewController)
{
	ABLOG_S(Warning);
	Super::PossessedBy(NewController);
}

