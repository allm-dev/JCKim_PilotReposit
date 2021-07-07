// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CAMERABOOM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	CameraBoom->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(CameraBoom);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-88),
		FRotator(0,-90,0));
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->SetRelativeRotation(FRotator(-15,0,0));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>SK_Cardboard(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if(SK_Cardboard.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Cardboard.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBP.WarriorAnimBP_C"));
	if(WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	SetControlMode(EControlMode::DIABLO);

	BoomLengthSpeed = 3.0f;
	BoomRotationSpeed = 10.0f;

	GetCharacterMovement()->JumpZVelocity = 800.0f;
}

void AMyCharacter::SetControlMode(EControlMode NewControlMode)
{
	CurrentControlMode = NewControlMode;
	
	switch(CurrentControlMode)
	{
		case EControlMode::GTA:
			
			//CameraBoom->TargetArmLength = 450.f;
			//CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);
			BoomLengthTo = 450.0f;
			CameraBoom->bUsePawnControlRotation = true;
			CameraBoom->bInheritPitch = true;
			CameraBoom->bInheritRoll=true;
			CameraBoom->bInheritYaw=true;

			//enable collision check
			CameraBoom->bDoCollisionTest=true;

			// do not take over pc rotator val to pawn transform rotation
			bUseControllerRotationYaw=false;

			//auto rotation toward character movement vector
			GetCharacterMovement()->bOrientRotationToMovement=true;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->RotationRate = FRotator(0,720,0);
		break;

		case EControlMode::DIABLO:

			//CameraBoom->TargetArmLength = 800.0f;
			//CameraBoom->SetRelativeRotation(FRotator(-45.0f,0,0));
			BoomLengthTo = 800.0f;
			BoomRotationTo =  FRotator(-45,0,0);
			CameraBoom->bUsePawnControlRotation = false;
			CameraBoom->bInheritPitch = false;
			CameraBoom->bInheritRoll = false;
			CameraBoom -> bInheritYaw = false;
			CameraBoom->bDoCollisionTest = false;
		
			//character rotation Yaw follows current pc coherent input
			bUseControllerRotationYaw = false;

			//rotation monitoring
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->RotationRate = FRotator(0,720,0);
		break;
	}
}


// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraBoom->TargetArmLength =FMath::FInterpTo(CameraBoom->TargetArmLength, BoomLengthTo, DeltaTime, BoomLengthSpeed);

	switch(CurrentControlMode)
	{
		case EControlMode::DIABLO:
			CameraBoom->SetRelativeRotation(FMath::RInterpTo(CameraBoom->GetRelativeRotation(), BoomRotationTo, DeltaTime, BoomRotationSpeed));
		break;

		default:
		break;
	}
	switch(CurrentControlMode)
	{
		case EControlMode::DIABLO:
			//이동 입력이 있을때
			if(DirectionToMove.SizeSquared() > 0.0f)
			{
				//이동 방향 벡터(정면)로 pc 회전값 추출 
				GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
				AddMovementInput(DirectionToMove);
			}
		break;

		default:
		break;
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AMyCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AMyCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMyCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMyCharacter::LookUp);

	PlayerInputComponent->BindAction(TEXT("ControlModeShift"), EInputEvent::IE_Pressed, this, &AMyCharacter::ControlModeShift);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
}

void AMyCharacter::UpDown(float NewAxisVal)
{
	switch(CurrentControlMode)
	{
		case EControlMode::GTA:
			//AddMovementInput(GetActorForwardVector(), NewAxisVal);
			AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisVal);
		break;

		case EControlMode::DIABLO:
			DirectionToMove.X = NewAxisVal;
		break;
			
	}
}

void AMyCharacter::LeftRight(float NewAxisVal)
{
	switch(CurrentControlMode)
	{
		case EControlMode::GTA:
			//AddMovementInput(GetActorRightVector(), NewAxisVal);
			AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisVal);
		break;

		case EControlMode::DIABLO:
			DirectionToMove.Y = NewAxisVal;
		break;
			
	}
	
}

void AMyCharacter::Turn(float NewAxisVal)
{
	switch(CurrentControlMode)
	{
		case EControlMode::GTA:
			//pc의 컨트롤 회전 입력값에 바인딩
			AddControllerYawInput(NewAxisVal);
		break;

		default:
			break;
	}
	
}
void AMyCharacter::LookUp(float NewAxisVal)
{
	switch(CurrentControlMode)
	{
		case EControlMode::GTA:
			//pc의 컨트롤 회전 입력값에 바인딩
			AddControllerPitchInput(NewAxisVal);
		break;

		default:
			break;
	}
}

void AMyCharacter::ControlModeShift()
{
	switch(CurrentControlMode)
	{
		case EControlMode::GTA:
			GetController()->SetControlRotation(GetActorRotation());
			SetControlMode(EControlMode::DIABLO);
		break;

		case EControlMode::DIABLO:
			GetController()->SetControlRotation(CameraBoom->GetRelativeRotation());
			SetControlMode(EControlMode::GTA);
		break;
	}
}




