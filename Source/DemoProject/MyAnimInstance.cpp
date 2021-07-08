// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"

UMyAnimInstance::UMyAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;

	//몽타주 생성
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage"));
	if(ATTACK_MONTAGE.Succeeded())
	{
		//Fobject instancing
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//get the owner(parent) of skeletal mesh of this animation == pawn.
	auto Pawn = TryGetPawnOwner();
	if(::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();

		//downcasting to use member func of ACharacter class
		auto Character = Cast<ACharacter>(Pawn);
		if(Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
	
}

void UMyAnimInstance::PlayAttackMontage()
{
	//no need, delegate checks its callback timing.
	//if(!Montage_IsPlaying(AttackMontage))
	
	Montage_Play(AttackMontage, 1.0f);
	
}

void UMyAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	MYCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

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

