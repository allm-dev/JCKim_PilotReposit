// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoFPSGameInstance.h"

UDemoFPSGameInstance::UDemoFPSGameInstance()
{
	for (int i = 0; i < EWeaponRootClassKey::WeaponRootClassKeyEnd; i++)
	{
		WeaponRootClass.Add(static_cast<EWeaponRootClassKey>(i), nullptr);
	}

	for (int i = 0; i < EWeaponClassKey::WeaponClassEnd; i++)
	{
		WeaponClass.Add(static_cast<EWeaponClassKey>(i), nullptr);
	}
	
}
