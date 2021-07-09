// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DemoProject.h"
#include "Blueprint/UserWidget.h"
#include "MyCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UMyCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindCharacterStat(class UMyCharacterStatComponent* NewCharacterStat);

private:
	TWeakObjectPtr<class UMyCharacterStatComponent> CurrentCharacterStat;	
	
};
