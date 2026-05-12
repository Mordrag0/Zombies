// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTDecorator_Temperament.h"

bool UZBTDecorator_Temperament::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return  false;//Temperament == GetCharacter(OwnerComp)->GetTemperament();
}

