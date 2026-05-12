// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_EndActivity.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeAsyncExecutionContext.h"
#include "Characters/ZNPCharacter.h"
#include "ZTypes.h"


EStateTreeRunStatus FZSTTask_EndActivity::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character || !InstanceData.Character->IsPerformingActivity(ToCharacterActivity(InstanceData.Activity)))
	{
		return EStateTreeRunStatus::Failed;
	}
	if (!InstanceData.Character->IsEndingActivity(ToCharacterActivity(InstanceData.Activity)))
	{
		const bool bEndedImmediately = InstanceData.Character->TriggerEndingActivity(ToCharacterActivity(InstanceData.Activity));
		if (bEndedImmediately)
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}
	BIND_MULTICAST(InstanceData.OnActivityStoppedConnection, InstanceData.Character, InstanceData.Character->OnActivityStopped,
		[WeakContext = Context.MakeWeakExecutionContext(), Activity = ToCharacterActivity(InstanceData.Activity)](EZCharacterActivity StoppedActivity)
	{
		if (Activity == StoppedActivity)
		{
			WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
		}
	});
	return EStateTreeRunStatus::Running;
}

void FZSTTask_EndActivity::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.OnActivityStoppedConnection.Disconnect();
}

#if WITH_EDITOR
FText FZSTTask_EndActivity::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_ENUM_DISPLAY_TEXT(EZCharacterActivityBP, Activity);
}
#endif

