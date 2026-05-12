// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_Interact.h"
#include "StateTreeExecutionContext.h"
#include "Characters/ZNPCharacter.h"
#include "ZInteractable.h"
#include "StateTreeAsyncExecutionContext.h"

FZSTTask_Interact::FZSTTask_Interact()
{
	//bShouldStateChangeOnReselect = false;
}

EStateTreeRunStatus FZSTTask_Interact::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character)
	{
		return EStateTreeRunStatus::Failed;
	}
	const bool bSuccess = InstanceData.Character->StartInteractingWith(InstanceData.Interactable);
	if (!bSuccess)
	{
		return EStateTreeRunStatus::Failed;
	}
	BIND_MULTICAST(InstanceData.OnInteractionStoppedConnection, InstanceData.Character, InstanceData.Character->OnInteractionStopped,
		[WeakContext = Context.MakeWeakExecutionContext(), WeakTarget = TWeakInterfacePtr<IZInteractable>(InstanceData.Interactable)](TScriptInterface<IZInteractable> Character, TScriptInterface<IZInteractable> Target)
	{
		if (WeakTarget.GetObject() == Target.GetObject())
		{
			WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
		}
	});
	return EStateTreeRunStatus::Running;
}

void FZSTTask_Interact::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (Transition.CurrentRunStatus != EStateTreeRunStatus::Succeeded)
	{
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		if (!InstanceData.Character)
		{
			return;
		}
		InstanceData.Character->StopInteractingWith(InstanceData.Interactable);
	}
}

#if WITH_EDITOR
FText FZSTTask_Interact::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_INTERFACE_DISPLAY_TEXT(Interactable);
}
#endif

