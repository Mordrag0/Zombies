// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_Talk.h"
#include "StateTreeExecutionContext.h"
#include "Characters/ZCharacter.h"
#include "Characters/ZNPCharacter.h"
#include "StateTreeAsyncExecutionContext.h"
#include "UObject/WeakObjectPtrTemplates.h"


EStateTreeRunStatus FZSTTask_Talk::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character || !InstanceData.Target)
	{
		return EStateTreeRunStatus::Failed;
	}
	const bool bInteractionSuccess = InstanceData.Character->StartInteractingWith(InstanceData.Target);
	if (!bInteractionSuccess)
	{
		return EStateTreeRunStatus::Failed;
	}
	BIND_MULTICAST(InstanceData.OnDialogueEndedConnection, InstanceData.Character, InstanceData.Character->OnInteractionStopped, 
		[WeakContext = Context.MakeWeakExecutionContext(), WeakTarget = MakeWeakObjectPtr(InstanceData.Target)](TScriptInterface<IZInteractable> Character, TScriptInterface<IZInteractable> Target)
	{
		if (WeakTarget.Get() == Target.GetObject())
		{
			WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
		}
	});
	return EStateTreeRunStatus::Running;
}

void FZSTTask_Talk::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.OnDialogueEndedConnection.Disconnect();
	if (Transition.CurrentRunStatus != EStateTreeRunStatus::Succeeded)
	{
		if (InstanceData.Character && InstanceData.Target)
		{
			InstanceData.Character->StopInteractingWith(InstanceData.Target);
		}
	}
}

#if WITH_EDITOR
FText FZSTTask_Talk::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_OBJECT_DISPLAY_TEXT(Target);
}
#endif

