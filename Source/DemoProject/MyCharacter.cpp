// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "MyAnimInstance.h"
#include "MyWeapon.h"
#include "MyCharacterStatComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "MyCharacterWidget.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CAMERABOOM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UMyCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	CameraBoom->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(CameraBoom);
	HPBarWidget->SetupAttachment(GetMesh());

	GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-88),
		FRotator(0,-90,0));
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->SetRelativeRotation(FRotator(-15,0,0));
	HPBarWidget->SetRelativeLocation(FVector(0,0,180));
	//overlay screen ui mode
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/UI/UI_HPBar.UI_HPBar_C"));
	if(UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150,50));
	}

	//Skeletal mesh object ref
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>SK_Cardboard(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if(SK_Cardboard.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Cardboard.Object);
	}

	//skeletal mesh uses AnimInstance based BP
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	//AnimInstanceClass ref
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBP.WarriorAnimBP_C"));
	if(WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	//control mode default settings
	SetControlMode(EControlMode::DIABLO);

	//mode change camera movement rate default setting
	BoomLengthSpeed = 3.0f;
	BoomRotationSpeed = 10.0f;

	//character movement comp , jumping speed setting
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	//combo sys. default setting (idle state, max combo count, ready to new combo stage)
	IsAttacking = false;
	MaxCombo = 4;
	AttackEndComboState();

	//collision profile default setting
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MyCharacter"));

	//Attack attribute settings
	AttackRadius = 50.0f;
	AttackRange = 200.0f;

	/*
	//Weapon equipment without actor separation
	FName WeaponSocket(TEXT("hand_rSocket"));
	//if any socket named as such was found
	if(GetMesh()->DoesSocketExist(WeaponSocket))
	{
		Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
		// instancing prefab asset
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_BlackKnight/SK_Blade_BlackKnight.SK_Blade_BlackKnight"));
		if(SK_WEAPON.Succeeded())
		{
			Weapon->SetSkeletalMesh(SK_WEAPON.Object);
		}

		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}
	*/
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

void AMyCharacter::SetWeapon(AMyWeapon* NewWeapon)
{
	MYCHECK(nullptr != NewWeapon && nullptr == CurrentWeapon);
	//weapon equipment as an independent actor
	FName WeaponSocket(TEXT("hand_rSocket"));
	if(nullptr!= NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
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

void AMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MyAnim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());

	//cast fail alarm
	MYCHECK(MyAnim != nullptr);

	//delegate connection ... when montage of MyAnimInstance ends its play, call bind func  == callback
	MyAnim->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnAttackMontageEnded);

	MyAnim->OnNextAttackCheck.AddLambda([this]()
	{
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		//default == combo-disabled
		CanNextCombo = false;

		//if combo input existed, combo-enabled
		if(IsComboInputOn)
		{
			AttackStartComboState();
			MyAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	//delegate uses AddUObject to bind non-UFUNCTION funcs(non-serialized func binding)
	MyAnim->OnAttackHitCheck.AddUObject(this, &AMyCharacter::AttackCheck);

	CharacterStat->OnHPIsZero.AddLambda([this]()
	{
		ABLOG(Warning, TEXT("OnHPIsZero"));
		MyAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	});

	auto CharacterWidget = Cast<UMyCharacterWidget>(HPBarWidget->GetUserWidgetObject());
	if(nullptr != CharacterWidget)
	{
		CharacterWidget->BindCharacterStat(CharacterStat);
	}
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	//superior func execution
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//log damage execution details
	ABLOG(Warning, TEXT("Actor : %s took Damage %f"), *GetName(), FinalDamage);

	CharacterStat->SetDamage(FinalDamage);
	return FinalDamage;
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//axis mapping
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AMyCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AMyCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMyCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMyCharacter::LookUp);

	//action mapping
	PlayerInputComponent->BindAction(TEXT("ControlModeShift"), EInputEvent::IE_Pressed, this, &AMyCharacter::ControlModeShift);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AMyCharacter::Attack);
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

//Left-Click action key bound F.
void AMyCharacter::Attack()
{
	//left click input when character state == attack, meaning combo is ongoing
	if(IsAttacking)
	{
		//assertion if CurrentCombo count is not in range 1~4
		MYCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		// if there's next combo attack montage section.
		if(CanNextCombo)
		{
			//combo allowance -> waits for OnNextAttackCheck callback to enable combo sys.
			IsComboInputOn = true;
		}
	}
	//left click input when character state == idle/run etc, meaning combo starting stage
	else
	{
		//if CurrentCombo count is not on its starting point -> halt program
		MYCHECK(CurrentCombo == 0);
		AttackStartComboState();
		MyAnim->PlayAttackMontage();
		MyAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking=true;		
	}
}

void AMyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// attack false 인데 들어오면 안됨
	MYCHECK(IsAttacking);
	//AttackEnd 실행 여부 체크, if called, CurrentCombo count should be > 0
	MYCHECK(CurrentCombo>0);
	//release attack state
	IsAttacking = false;
	AttackEndComboState();
}

void AMyCharacter::AttackStartComboState()
{
	//OnNextAttackCheck callback timing에 true인 경우 콤보가 이어짐
	CanNextCombo =	true;
	IsComboInputOn = false;
	//combo start stage should start within a certain range of numbers; 0<= x<= MaxCom-1
	MYCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo-1));
	//clamping, always plus one to currentCombo variable, but stay its val between min and max range 1~4
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo+1, 1, MaxCombo);
}

void AMyCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AMyCharacter::AttackCheck()
{
	FHitResult HitResult;
	//collision trace query settings (tag name, complexity, ignore)
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		//trace check 내 검출된 결과 데이터 struct
		HitResult,
		//start location of checking trace == actor now location
		GetActorLocation(),
		//end location of checking trace == forward vec of actor * 200
		GetActorLocation()+GetActorForwardVector()*AttackRange,
		//rotation of checker == default
		FQuat::Identity,
		//trace collision channel to use
		ECollisionChannel::ECC_GameTraceChannel2,
		//primitive object to use in collision check trace
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	//attack range collision trace debug drawing
#if ENABLE_DRAW_DEBUG
	//end of trace
	FVector TraceVec = GetActorForwardVector()*AttackRange;
	//middle point of trace
	FVector Center = GetActorLocation() + TraceVec*0.5f;
	//capsule whole height /2
	float HalfHeight = AttackRange*0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	//충돌하면 그린 아니면 레드
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(
		GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime
	);

#endif
	

	//if hit something
	if(bResult)
	{
		//if hit actor is valid (actor declaration is TWeak*)
		if(HitResult.Actor.IsValid())
		{
			//log hit actor name
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

			//damage framework
			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(CharacterStat->GetAttack(), DamageEvent, GetController(), this);
		}
	}
}



