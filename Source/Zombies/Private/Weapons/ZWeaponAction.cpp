// Copyright 2026 Luka Markuš. All rights reserved.


#include "Weapons/ZWeaponAction.h"
#include "Weapons/ZWeapon.h"

DEFINE_LOG_CATEGORY(LogZWeaponAction);

void UZWeaponAction::Initialize(AZWeapon* InWeapon)
{
	if (bInitialized)
	{
		return;
	}
	bInitialized = true;

	Weapon = InWeapon;
}

bool UZWeaponAction::CanFire() const
{
	return Weapon->CanFire();
}

void UZWeaponAction::FirePressed()
{
	bHeldDown = true;
}

void UZWeaponAction::FireComplete()
{

}

void UZWeaponAction::FireReleased()
{
	bHeldDown = false;
}

