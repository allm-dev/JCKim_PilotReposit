// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DemoProject.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class DEMOPROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 컨트롤 모드 변수 타입
	enum class EControlMode
	{
		GTA,
		DIABLO
	};;

	void SetControlMode(EControlMode NewControlMode);

	//Starts with GTA Mode
	EControlMode CurrentControlMode = EControlMode::GTA;

	//입력 방향 벡터 조합 변수
	FVector DirectionToMove = FVector::ZeroVector;

	float BoomLengthTo = 0.0f;
	FRotator BoomRotationTo = FRotator::ZeroRotator;
	float BoomLengthSpeed = 0.0f;
	float BoomRotationSpeed = 0.0f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	UCameraComponent* Camera;

private:
	void UpDown(float NewAxisVal);
	void LeftRight(float NewAxisVal);
	void LookUp(float NewAxisVal);
	void Turn(float NewAxisVal);

	void ControlModeShift();
};