// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_EndInteract.h"
#include "StateTreeAsyncExecutionContext.h"
#include "StateTreeExecutionContext.h"
#include "Characters/ZNPCharacter.h"
#include "ZTypes.h"

EStateTreeRunStatus FZSTTask_EndInteract::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character || !InstanceData.Interactable || InstanceData.Activity == EZCharacterActivityBP::None)
	{
		return EStateTreeRunStatus::Failed;
	}
	TScriptInterface<IZInteractable> ActivityInteractable = InstanceData.Character->GetInteractionTarget(ToCharacterActivity(InstanceData.Activity));
	if (ActivityInteractable != InstanceData.Interactable)
	{
		return EStateTreeRunStatus::Failed;
	}
	BIND_MULTICAST(InstanceData.OnInteractionEndedConnection, InstanceData.Character, InstanceData.Character->OnActivityStopped, 
		[WeakContext = Context.MakeWeakExecutionContext(), Activity = ToCharacterActivity(InstanceData.Activity)](EZCharacterActivity StoppedActivity)
	{
		if (Activity == StoppedActivity)
		{
			WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
		}
	});
	return EStateTreeRunStatus::Running;
}

void FZSTTask_EndInteract::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.OnInteractionEndedConnection.Disconnect();
}

#if WITH_EDITOR
FText FZSTTask_EndInteract::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return JoinParameters({ GET_INTERFACE_DISPLAY_TEXT(Interactable), GET_ENUM_DISPLAY_TEXT(EZCharacterActivityBP, Activity)});
}
#endif

