// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ZPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	AZPlayerCameraManager();

	void SetDefaultFOV(float FOV);
	void SetTargetFOV(float NewFOV);
	void ResetFOV();

	virtual void UpdateCamera(float DeltaTime) override;

	void SetScopedSensitivityScale(float InScopeSensitivityScale) { ScopedSensitivityScale = InScopeSensitivityScale; }

protected:

	UPROPERTY(EditDefaultsOnly)
	float NormalFOV;

	float TargetingFOV;
	float TargetSensAdjustment;

	const float HalfRad2Deg = 0.0174533 / 2;
	float NormalFOVTanInv;

	virtual void BeginPlay() override;

	bool bUpdating;
	float ScopedSensitivityScale;
};

