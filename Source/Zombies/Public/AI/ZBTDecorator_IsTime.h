// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZBTDecorator_BlackboardBase.h"
#include "ZBTDecorator_IsTime.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTDecorator_IsTime : public UZBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UZBTDecorator_IsTime();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	UPROPERTY(EditAnywhere)
	int32 TimeFrom;

	UPROPERTY(EditAnywhere)
	int32 TimeTo;
};

