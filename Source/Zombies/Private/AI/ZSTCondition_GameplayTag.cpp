// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_GameplayTag.h"
#include "StateTreeExecutionContext.h"


bool FZSTCondition_IsGameplayTagValid::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const bool bResult = InstanceData.Tag.IsValid();
	return bResult ^ bInvert;
}

#if WITH_EDITOR
FText FZSTCondition_IsGameplayTagValid::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_GAMEPLAYTAG_DISPLAY_TEXT(Tag);
}
#endif
