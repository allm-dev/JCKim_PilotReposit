// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"

UMyAnimInstance::UMyAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;

	//몽타주 생성
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage"));
	if(ATTACK_MONTAGE.Succeeded())
	{
		//Fobject instancing
		AttackMontage = ATTACK_MONTAGE.Object;
	}

	
}

//works in Tick procedure, to update values of a certain group of member vars (CurrentPawnSpeed,  IsInAir)
void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//get the owner(parent) of skeletal mesh of this animation == pawn.
	auto Pawn = TryGetPawnOwner();

	//if pawn is on GC collection in due course, return.
	if(!::IsValid(Pawn)) return;

	//if dead, no animation-state triggers update
	if(!IsDead)
	{
		//Run state trigger updates
		CurrentPawnSpeed = Pawn->GetVelocity().Size();

		//downcasting to use member func of ACharacter class
		auto Character = Cast<ACharacter>(Pawn);
		if(Character)
		{
			//Jump state trigger updates
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
	
}

//plays each section of combo attack anim montage
void UMyAnimInstance::PlayAttackMontage()
{
	//no need, delegate checks its callback timing.
	//if(!Montage_IsPlaying(AttackMontage))

	//should not be called upon dead state
	MYCHECK(!IsDead)
	//execution code
	Montage_Play(AttackMontage, 1.0f);
	
}

//change section of combo attack montage
void UMyAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	//should not be called upon non-attacking state
	MYCHECK(Montage_IsPlaying(AttackMontage));
	//should not be called upon dead state
	MYCHECK(!IsDead);
	//driving code
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

//can be made const, merit?
void UMyAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UMyAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UMyAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	MYCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

