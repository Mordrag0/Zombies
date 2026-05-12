// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTTask_Base.h"
#include "ZBTTask_SetMovementMode.generated.h"

enum class EZGaitState : uint8;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTTask_SetMovementMode : public UZBTTask_Base
{
	GENERATED_BODY()
	
public:
	UZBTTask_SetMovementMode();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere)
	EZGaitState State;
};

