// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZSTTask_GetPathWaypointLocation.generated.h"

class IZInteractable;
class AZNPCAIController;

USTRUCT()
struct FZSTTask_GetPathWaypointLocationInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Context")
	TObjectPtr<AZNPCAIController> Controller;

	UPROPERTY(VisibleAnywhere, Category = "Output")
	FTransform Destination;
};

USTRUCT(meta = (DisplayName = "Get path waypoint location", Category = "AI"))
struct FZSTTask_GetPathWaypointLocation : public FZStateTreeAITaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_GetPathWaypointLocationInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};



