// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ZFireMode.h"
#include "ZFireModeBurst.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZFireModeBurst : public UZFireMode
{
	GENERATED_BODY()
	
public:
	virtual bool CanFire() const override;
	virtual void FirePressed() override;
	virtual void FireComplete() override;
	virtual void Fire() override;
protected:
	void StartBurst();
	void BurstComplete();

	UPROPERTY(EditDefaultsOnly)
	float BurstTime;

	UPROPERTY(EditDefaultsOnly)
	int32 ShotsPerBurst;

	int32 BurstShotsRemaining;

	FTimerHandle TimerHandle_Burst;
};

