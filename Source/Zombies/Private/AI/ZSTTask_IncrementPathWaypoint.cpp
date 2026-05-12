// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_IncrementPathWaypoint.h"
#include "StateTreeExecutionContext.h"
#include "ZGameState.h"
#include "AI/ZPath.h"
#include "AI/ZWaypoint.h"
#include "AI/ZNPCAIController.h"
#include "Characters/ZFPCharacter.h"


EStateTreeRunStatus FZSTTask_IncrementPathWaypoint::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Controller || !InstanceData.Player)
	{
		return EStateTreeRunStatus::Failed;
	}
	FZPathData& PathData = InstanceData.Controller->GetPathData();
	const bool bComplete = PathData.IncrementWaypointIndex();
	return bComplete ? EStateTreeRunStatus::Failed : EStateTreeRunStatus::Succeeded; // return whether we still have a valid path
}

