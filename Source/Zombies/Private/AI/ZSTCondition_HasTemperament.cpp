// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_HasTemperament.h"
#include "Characters/ZNPCharacter.h"
#include "StateTreeExecutionContext.h"
#include "ZTypes.h"

bool FZSTCondition_HasTemperament::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const bool bResult = InstanceData.NPC->GetTemperament() == InstanceData.Temperament;
	return bResult ^ bInvert;
}

#if WITH_EDITOR
FText FZSTCondition_HasTemperament::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_ENUM_DISPLAY_TEXT(EZTemperament, Temperament);
}
#endif


