// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_GetAssignedInteractable.h"
#include "StateTreeExecutionContext.h"
#include "Characters/ZNPCharacter.h"
#include "ZTypes.h"


FZSTTask_GetAssignedInteractableInstanceData::FZSTTask_GetAssignedInteractableInstanceData()
{
	Activity = EZCharacterActivityBP::None;
}

FZSTTask_GetAssignedInteractable::FZSTTask_GetAssignedInteractable()
{
	//bShouldStateChangeOnReselect = false;
}

EStateTreeRunStatus FZSTTask_GetAssignedInteractable::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character)
	{
		return EStateTreeRunStatus::Failed;
	}
	InstanceData.Interactable = InstanceData.Character->GetAssignedInteractable(ToCharacterActivity(InstanceData.Activity));
	return InstanceData.Interactable ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR
FText FZSTTask_GetAssignedInteractable::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_ENUM_DISPLAY_TEXT(EZCharacterActivityBP, Activity);
}
#endif

