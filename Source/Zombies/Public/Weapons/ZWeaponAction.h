// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ZWeaponAction.generated.h"

class AZWeapon;

DECLARE_LOG_CATEGORY_EXTERN(LogZWeaponAction, Log, All);

/**
 * 
 */
UCLASS(ClassGroup=(Custom))
class ZOMBIES_API UZWeaponAction : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(AZWeapon* InWeapon);

	virtual bool CanFire() const;
	virtual void FirePressed();
	virtual void FireComplete();
	virtual void FireReleased();

	bool IsHeldDown() const { return bHeldDown; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	float FireRate = 1.f;

	UPROPERTY()
	TObjectPtr<AZWeapon> Weapon;

	bool bInitialized;

	bool bHeldDown;
};

