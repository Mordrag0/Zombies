// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_GetPathWaypointLocation.h"
#include "AI/ZNPCAIController.h"
#include "ZGameState.h"
#include "AI/ZPath.h"
#include "AI/ZWaypoint.h"
#include "StateTreeExecutionContext.h"


EStateTreeRunStatus FZSTTask_GetPathWaypointLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Controller)
	{
		return EStateTreeRunStatus::Failed;
	}
	const FZPathData& PathData = InstanceData.Controller->GetPathData();
	if (!PathData.IsValid())
	{
		return EStateTreeRunStatus::Failed;
	}
	const AZWaypoint* Waypoint = PathData.Path->GetWaypoint(PathData.GetWaypointIndex());
	if (!Waypoint)
	{
		return EStateTreeRunStatus::Failed;
	}
	InstanceData.Destination = Waypoint->GetActorTransform();
	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FZSTTask_GetPathWaypointLocation::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_OBJECT_DISPLAY_TEXT(Controller);
}
#endif

