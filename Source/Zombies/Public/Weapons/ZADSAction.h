// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ZWeaponAction.h"
#include "ZADSAction.generated.h"

class AZPlayerCameraManager;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZADSAction : public UZWeaponAction
{
	GENERATED_BODY()
	
public:
	UZADSAction();

	virtual void Initialize(AZWeapon* InWeapon) override;

	virtual void FirePressed() override;
	virtual void FireReleased() override;

protected:
	void StartADS();
	void StopADS();
	
	UPROPERTY(EditDefaultsOnly, Category = "FireMode")
	float ADS_FOV;

	UPROPERTY()
	TObjectPtr<AZPlayerCameraManager> PCM;
};

