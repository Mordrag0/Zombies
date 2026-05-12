// Copyright 2026 Luka Markuš All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Movement/ZMovementModeTransition.h"
#include "ZMMTransition_ToSlide.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZMMTransition_ToSlide : public UZMovementModeTransition
{
	GENERATED_BODY()
	
public:
	virtual FTransitionEvalResult Evaluate_Implementation(const FSimulationTickParams& Params) const override;
};

