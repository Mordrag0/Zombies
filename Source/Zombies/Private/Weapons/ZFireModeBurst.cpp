// Copyright 2026 Luka Markuš. All rights reserved.


#include "Weapons/ZFireModeBurst.h"

bool UZFireModeBurst::CanFire() const
{
	if (!Super::CanFire())
	{
		return false;
	}

	return !GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Burst) || (BurstShotsRemaining > 0);
}

void UZFireModeBurst::FirePressed()
{
	const bool bCanFire = CanFire();
	UZWeaponAction::FirePressed(); // Don't fire immediately, we fire from StartBurst

	if (bCanFire)
	{
		StartBurst();
	}
}

void UZFireModeBurst::FireComplete()
{
	Super::FireComplete();

	if (CanFire())
	{
		Fire();
	}
}

void UZFireModeBurst::Fire()
{
	Super::Fire();
	BurstShotsRemaining -= 1;
}

void UZFireModeBurst::StartBurst()
{
	BurstShotsRemaining = ShotsPerBurst;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Burst, this, &ThisClass::BurstComplete, BurstTime);
	Fire();
}

void UZFireModeBurst::BurstComplete()
{
	if (bHeldDown)
	{
		StartBurst();
	}
}

