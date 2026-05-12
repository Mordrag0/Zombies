// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_GetPivotPoint.h"
#include "StateTreeExecutionContext.h"
#include "ZInteractable.h"


FZSTTask_GetPivotPoint::FZSTTask_GetPivotPoint()
{
}

EStateTreeRunStatus FZSTTask_GetPivotPoint::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Interactable)
	{
		return EStateTreeRunStatus::Failed;
	}
	InstanceData.PivotPoint = InstanceData.Interactable->GetPivotPoint();
	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FZSTTask_GetPivotPoint::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_OBJECT_DISPLAY_TEXT(Interactable.GetObject());
}
#endif


