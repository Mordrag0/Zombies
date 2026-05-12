// Copyright 2026 Luka Markuš. All rights reserved.


#include "Player/ZPlayerCameraManager.h"
#include "ZLog.h"
#include "Characters/ZFPCharacter.h"

AZPlayerCameraManager::AZPlayerCameraManager()
{
	bAlwaysApplyModifiers = true;

	NormalFOV = 90.f;
	TargetingFOV = 0.f;
	TargetSensAdjustment = 1.f;

	NormalFOVTanInv = 0.f;

	bUpdating = false;
	ScopedSensitivityScale = 0.f;
}

void AZPlayerCameraManager::SetDefaultFOV(float FOV)
{
	NormalFOV = FOV;
	DefaultFOV = FOV;
	NormalFOVTanInv = 1 / FMath::Tan(NormalFOV * HalfRad2Deg);
}

void AZPlayerCameraManager::SetTargetFOV(float NewFOV)
{
	TargetingFOV = NewFOV;
	TargetSensAdjustment = FMath::Tan(NewFOV / 2) / FMath::Tan(NormalFOV / 2);
	bUpdating = true;
}

void AZPlayerCameraManager::ResetFOV()
{
	TargetingFOV = NormalFOV;
	bUpdating = true;
}

void AZPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	if (bUpdating)
	{
		if (PCOwner && PCOwner->IsLocalController())
		{
			if (FMath::IsNearlyEqual(DefaultFOV, TargetingFOV))
			{
				bUpdating = false;
				DefaultFOV = TargetingFOV;
			}
			else
			{
				DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetingFOV, DeltaTime, 20.0f);
			}
			SetFOV(DefaultFOV);
			// UE_LOG(LogZombies, Log, TEXT("FOV %f"), DefaultFOV);
			// Here we adjust the sens with the FOV change
			// The equation to keep approximately the same sens on the new FOV:
			// tan(NewFov / 2) / tan(OldFov / 2)
			// And then lerp based on ScopedSensitivityScale [0-1]
			// 0 means we use what the equation gave us, 1 means no adjustment for FOV change and everything else is something in between
			float SensAdjustment = FMath::Lerp(FMath::Tan(DefaultFOV * HalfRad2Deg) * NormalFOVTanInv, 1.f, ScopedSensitivityScale);
			UE_LOG(LogZombies, VeryVerbose, TEXT("SensAdjustment %f"), SensAdjustment);
			AZFPCharacter* Character = PCOwner ? PCOwner->GetPawn<AZFPCharacter>() : nullptr; 
			if(Character)
			{
				Character->SetFOVSensAdjustment(SensAdjustment);
			}
		}
	}

	Super::UpdateCamera(DeltaTime);
}

void AZPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();

	TargetingFOV = NormalFOV;
	SetFOV(NormalFOV);
}

