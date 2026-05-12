// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZSTTask_IncrementPathWaypoint.generated.h"

class AZFPCharacter;
class AZNPCAIController;

USTRUCT()
struct FZSTTask_IncrementPathWaypointInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Context")
	TObjectPtr<AZNPCAIController> Controller;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	TObjectPtr<AZFPCharacter> Player;
};

USTRUCT(meta = (DisplayName = "Increment path waypoint index", Category = "AI"))
struct FZSTTask_IncrementPathWaypoint : public FZStateTreeAITaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_IncrementPathWaypointInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	STATETREE_NODE_DESCRIPTION()
};



