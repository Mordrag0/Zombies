// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTDecorator_BlackboardBase.h"
#include "ZBTDecorator_CanInteract.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTDecorator_CanInteract : public UZBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UZBTDecorator_CanInteract();
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;
};

