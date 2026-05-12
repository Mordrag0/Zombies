// Copyright 2026 Luka Markuš. All rights reserved.


#include "Weapons/ZADSAction.h"
#include "ZPlayerController.h"
#include "Weapons/ZWeapon.h"
#include "Characters/ZCharacter.h"
#include "Player/ZPlayerCameraManager.h"

UZADSAction::UZADSAction()
{
	ADS_FOV = 20.f;
}

void UZADSAction::Initialize(AZWeapon* InWeapon)
{
	Super::Initialize(InWeapon);
	
	AZPlayerController* PC = Weapon->GetCharacterOwner()->GetController<AZPlayerController>();
	PCM = PC ? PC->GetPlayerCameraManager() : nullptr;
	ensure(!PC || !PC->IsPlayerController() || PCM);
}

void UZADSAction::FirePressed()
{
	Super::FirePressed();

	StartADS();
}

void UZADSAction::FireReleased()
{
	Super::FireReleased();

	StopADS();
}

void UZADSAction::StartADS()
{
	if (PCM)
	{
		PCM->SetTargetFOV(ADS_FOV);
	}
	Weapon->GetCharacterOwner()->SetADSing(true);
}

void UZADSAction::StopADS()
{
	if (PCM)
	{
		PCM->ResetFOV();
	}
	Weapon->GetCharacterOwner()->SetADSing(false);
}

