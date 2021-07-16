// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ItemRoot.h"
#include "Engine/GameInstance.h"
#include "DemoFPSGameInstance.generated.h"

/**
 * 
 */

UENUM(Blueprintable)
enum ItemRootClassKey
{
	Weapon1Root = 0,
	Weapon2Root, 
	Weapon3Root,
	ItemRootClassKeyEnd
	
};



UCLASS()
class DEMOFPS_API UDemoFPSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UDemoFPSGameInstance();

	UPROPERTY(EditDefaultsOnly, Category=ItemRootClass)
	TMap<TEnumAsByte<ItemRootClassKey>, TSubclassOf<AItemRoot>> ItemRootClass;

	TMap<TEnumAsByte<ItemRootClassKey>, TSubclassOf<AItemRoot>> GetItemRootClass() const {return ItemRootClass;}
};
