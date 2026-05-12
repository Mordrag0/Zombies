// Copyright 2026 Luka Markuš. All rights reserved.


#include "Weapons/ZFireMode.h"
#include "Weapons/ZWeapon.h"

UZFireMode::UZFireMode()
{
	SpreadTimeAddedPerShot = 0.5f;
}

void UZFireMode::Initialize(AZWeapon* InWeapon)
{
	if (bInitialized)
	{
		return;
	}
	Super::Initialize(InWeapon);

	Spread->GetTimeRange(MinSpreadTime, MaxSpreadTime);
}

bool UZFireMode::CanFire() const
{
	if (!Super::CanFire())
	{
		return false;
	}
	return !GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Fire);
}

void UZFireMode::FirePressed()
{
	const bool bCanFire = CanFire();
	Super::FirePressed();
	
	if (bCanFire)
	{
		Fire();
	}
}

void UZFireMode::FireComplete()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
	Super::FireComplete();

	Weapon->FireComplete();
}

void UZFireMode::FireReleased()
{
	Super::FireReleased();
}

void UZFireMode::Fire()
{
	Weapon->PerformShot();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fire, this, &ThisClass::FireComplete, FireRate, false);
	SpreadTimeAfterLastShot = GetSpreadTime() + SpreadTimeAddedPerShot;
	LastFireTime = GetWorld()->GetTimeSeconds();
}

float UZFireMode::GetSpreadTime() const
{
	const float WorldTime = GetWorld()->GetTimeSeconds();
	const float TimePassed = WorldTime - LastFireTime;
	return FMath::Clamp(SpreadTimeAfterLastShot - TimePassed, MinSpreadTime, MaxSpreadTime);
}

float UZFireMode::GetSpread() const
{
	const float SpreadTime = GetSpreadTime();
	return Spread->GetFloatValue(SpreadTime);
}

