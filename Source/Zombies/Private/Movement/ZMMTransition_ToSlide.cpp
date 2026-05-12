// Copyright 2026 Luka Markuš All rights reserved.


#include "Movement/ZMMTransition_ToSlide.h"
#include "MoverSimulationTypes.h"
#include "Movement/ZCharacterMoverComponent.h"


FTransitionEvalResult UZMMTransition_ToSlide::Evaluate_Implementation(const FSimulationTickParams& Params) const
{
	const UZCharacterMoverComponent* MoverComp = Cast<UZCharacterMoverComponent>(Params.MovingComps.MoverComponent.Get());
	if (!MoverComp)
	{
		return FTransitionEvalResult::NoTransition;
	}
	
	if (MoverComp->IsCrouching())
	{
		
	}
	return FTransitionEvalResult::NoTransition;
}

