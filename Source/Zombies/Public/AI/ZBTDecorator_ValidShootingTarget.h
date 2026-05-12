// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTDecorator_BlackboardBase.h"
#include "ZBTDecorator_ValidShootingTarget.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTDecorator_ValidShootingTarget : public UZBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UZBTDecorator_ValidShootingTarget();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};

