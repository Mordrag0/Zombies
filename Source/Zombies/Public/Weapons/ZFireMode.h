// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZWeaponAction.h"
#include "ZFireMode.generated.h"

class UCurveFloat;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZFireMode : public UZWeaponAction
{
	GENERATED_BODY()
	
public:
	UZFireMode();

	virtual void Initialize(AZWeapon* InWeapon) override;

	virtual bool CanFire() const override;
	virtual void FirePressed() override;
	virtual void FireComplete() override;
	virtual void FireReleased() override;
	virtual void Fire();
	
	virtual float GetSpread() const;

protected:
	float GetSpreadTime() const;

	FTimerHandle TimerHandle_Fire;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire Mode")
	UCurveFloat* Spread;
	
	// How far to the right we go on the curve with each shot
	UPROPERTY(EditDefaultsOnly, Category = "Fire Mode")
	float SpreadTimeAddedPerShot;

	float SpreadTimeAfterLastShot;
	float LastFireTime;

	float MinSpreadTime;
	float MaxSpreadTime;
};

