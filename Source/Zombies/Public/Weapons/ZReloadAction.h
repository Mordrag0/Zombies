// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ZWeaponAction.h"
#include "ZReloadAction.generated.h"

class AZRangedWeapon;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZReloadAction : public UZWeaponAction
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(AZWeapon* InWeapon);

	virtual bool CanFire() const;
	virtual void FirePressed() override;
	virtual void FireComplete() override;

protected:
	UPROPERTY()
	TObjectPtr<AZRangedWeapon> RangedWeapon;
};

