// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmunitionComp.h"

// Sets default values for this component's properties
UAmmunitionComp::UAmmunitionComp()
{
	Ammo0Count =30;
	Ammo1Count = 7;
	Ammo2Count = 10;
	GrenadeCount = 3;
}

int32 UAmmunitionComp::GetAmmoXCount(int32 AmmoId) const
{
	int32 Res =0;
	
	switch (AmmoId)
	{
	case 0:
		Res =Ammo0Count;
		break;
	case 1:
		Res = Ammo1Count;
		break;
	case 2:
		Res = Ammo2Count;
		break;
	default:
		Res =0;
		break;
	}
	return Res;
}

void UAmmunitionComp::DeductAmmoXCount(int32 AmmoId, int32 Amount)
{
	if (Amount < 0)
	{
		return;
	}
	
	switch (AmmoId)
	{
		case 0:
			Ammo0Count = FMath::Clamp(Ammo0Count - Amount, 0, Ammo0Count);
			break;
		case 1:
			Ammo1Count = FMath::Clamp(Ammo1Count - Amount, 0, Ammo1Count);
			break;
		case 2:
			Ammo2Count = FMath::Clamp(Ammo2Count - Amount, 0, Ammo2Count);
			break;
		default:
			break;
	}
}

void UAmmunitionComp::AddAmmoXCount(int32 AmmoId, int32 Amount)
{
	if (Amount < 0)
	{
		return;
	}
	
	switch (AmmoId)
	{
	case 0:
		Ammo0Count += Amount;
		break;
		
	case 1:
		Ammo1Count += Amount;
		break;

	case 2:
		Ammo2Count += Amount;
		break;

	default:
		break;
	}
}

void UAmmunitionComp::AddCurrentGrenadeCount(int32 Amount)
{
	if(Amount < 0)
	{
		return;
	}

	GrenadeCount += Amount;
}

void UAmmunitionComp::DeductCurrentGrenadeCount(int32 Amount)
{
	if(Amount < 0)
	{
		return;
	}

	GrenadeCount = FMath::Clamp(GrenadeCount - Amount , 0, GrenadeCount);
}
