// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_IsTime.h"
#include "StateTreeExecutionContext.h"

bool FZSTCondition_IsTime::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	return (InstanceData.NewHour == InstanceData.Hour);
}

#if WITH_EDITOR
FText FZSTCondition_IsTime::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::Format(INVTEXT("{0} == {1}"), GET_NUMBER_DISPLAY_TEXT(Hour), GET_NUMBER_DISPLAY_TEXT(NewHour));
}
#endif

