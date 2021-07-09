// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DemoProject.h"
#include "Components/ActorComponent.h"
#include "MyCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEMOPROJECT_API UMyCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//component's post-initialize-components func
	virtual void InitializeComponent() override;

public:
	void SetNewLevel(int32 NewLevel);
	void SetDamage(float NewDamage);
	float GetAttack();

	FOnHPIsZeroDelegate OnHPIsZero;

private:
	struct FMyCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category=Stat, Meta = (AllowPrivateAccess = true))
	int32 Level;

	//exclude from serialization process of ue4
	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat, Meta=(AllowPrivateAccess=true))
	float CurrentHP;
};
