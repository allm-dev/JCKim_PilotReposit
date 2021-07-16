// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoFPSGameInstance.h"

UDemoFPSGameInstance::UDemoFPSGameInstance()
{
	for(int i =0; i<ItemRootClassKey::ItemRootClassKeyEnd; i++)
	{
		ItemRootClass.Add(static_cast<ItemRootClassKey>(i), nullptr);
	}
	
}
