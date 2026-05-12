// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_CanEnterPriorityState.h"
#include "AI/ZNPCAIController.h"
#include "StateTreeExecutionContext.h"
#include "ZTypes.h"


bool FZSTCondition_CanEnterPriorityState::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Controller)
	{
		return false;
	}
	return InstanceData.Controller->CanEnterPriorityState(StatePriority);
}

#if WITH_EDITOR
FText FZSTCondition_CanEnterPriorityState::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(ENUM_TO_STRING(EZNPCStatePriority, StatePriority));
}
#endif

