// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ZCharacterProgressionData.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZCharacterProgressionData : public UDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UCurveFloat> HealthCurve;      // Health at each level
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UCurveFloat> DamageCurve;      // Damage at each level
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UCurveFloat> XPRewardCurve;    // XP reward for killing at each level
};

