// Copyright 2026 Luka Markuš. All rights reserved.


#include "Weapons/ZReloadAction.h"
#include "Weapons/ZRangedWeapon.h"

void UZReloadAction::Initialize(AZWeapon* InWeapon)
{
	Super::Initialize(InWeapon);

	if (AZRangedWeapon* CastedWeapon = Cast<AZRangedWeapon>(InWeapon))
	{
		RangedWeapon = CastedWeapon;
	}
	else
	{
		UE_LOG(LogZWeaponAction, Error, TEXT("UZReloadAction can only be used with AZRangedWeapon"));
	}
}

bool UZReloadAction::CanFire() const
{
	return RangedWeapon->IsIdle() && RangedWeapon->CanReload();
}

void UZReloadAction::FirePressed()
{
	const bool bCanFire = CanFire();

	Super::FirePressed();

	if (bCanFire)
	{
		RangedWeapon->StartReload(FireRate);
	}
}

void UZReloadAction::FireComplete()
{
	Super::FireComplete();
}

