// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Movement/ZSmoothWalkingMode.h"

#define MAX_VARIANCE_ALLOWED 0.05f; // #ZTODO

#define ECC_Interactable ECC_GameTraceChannel2
#define ECC_WeaponTrace ECC_GameTraceChannel3

namespace ZMovementMode
{
    static const FName Walking = DefaultModeNames::Walking;
    static const FName Falling = DefaultModeNames::Falling;
    static const FName Flying = DefaultModeNames::Flying;
    static const FName Swimming = DefaultModeNames::Swimming;
    static const FName Sliding("Sliding");
}
