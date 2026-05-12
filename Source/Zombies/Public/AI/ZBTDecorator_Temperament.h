// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZBTDecorator_Base.h"
#include "ZBTDecorator_Temperament.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTDecorator_Temperament : public UZBTDecorator_Base
{
	GENERATED_BODY()
	
protected:

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;
};

