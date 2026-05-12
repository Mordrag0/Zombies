// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTTask_BlackboardBase.h"
#include "ZBTTask_SnapLocation.generated.h"

class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTTask_SnapLocation : public UZBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UZBTTask_SnapLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

