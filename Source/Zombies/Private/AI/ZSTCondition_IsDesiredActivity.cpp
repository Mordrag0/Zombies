// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_IsDesiredActivity.h"
#include "AI/ZNPCAIController.h"
#include "ZTypes.h"
#include "StateTreeExecutionContext.h"
#include "ZGameState.h"

FZSTCondition_IsDesiredActivityInstanceData::FZSTCondition_IsDesiredActivityInstanceData()
{
	Activity = EZCharacterActivityBP::None;
}

bool FZSTCondition_IsDesiredActivity::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Controller)
	{
		return false;
	}
	AZGameState* GS = Context.GetWorld()->GetGameState<AZGameState>();
	if (!GS)
	{
		return false;
	}
	return (InstanceData.Controller->GetDesiredActivity(GS->GetHour()) == ToCharacterActivity(InstanceData.Activity)) ^ bInvert;
}

#if WITH_EDITOR
FText FZSTCondition_IsDesiredActivity::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_ENUM_DISPLAY_TEXT(EZCharacterActivityBP, Activity);
}
#endif

