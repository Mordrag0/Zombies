// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_Object.h"
#include "StateTreeExecutionContext.h"

bool FZSTCondition_IsObjectValid::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const bool bResult = IsValid(InstanceData.Object);
	return bResult ^ bInvert;
}

#if WITH_EDITOR
FText FZSTCondition_IsObjectValid::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_OBJECT_DISPLAY_TEXT(Object);
}
#endif

