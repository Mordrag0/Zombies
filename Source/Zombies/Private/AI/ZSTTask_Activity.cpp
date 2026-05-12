// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_Activity.h"
#include "Characters/ZNPCharacter.h"
#include "ZTypes.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeAsyncExecutionContext.h"


EStateTreeRunStatus FZSTTask_Activity::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.NPC || !IsInteractionActivity(ToCharacterActivity(InstanceData.Activity)))
	{
		return EStateTreeRunStatus::Failed;
	}
	BIND_MULTICAST(InstanceData.OnActivityStoppedConnection, InstanceData.NPC, InstanceData.NPC->OnActivityStopped, 
		[WeakContext = Context.MakeWeakExecutionContext(), Activity = ToCharacterActivity(InstanceData.Activity)](EZCharacterActivity StoppedActivity)
	{
		if (Activity == StoppedActivity)
		{
			WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
		}
	});
	BIND_MULTICAST(InstanceData.OnActivityEndingConnection, InstanceData.NPC, InstanceData.NPC->OnActivityEnding,
		[WeakContext = Context.MakeWeakExecutionContext(), Activity = ToCharacterActivity(InstanceData.Activity)](EZCharacterActivity EndingActivity)
	{
		if (Activity == EndingActivity)
		{
			WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
		}
	});
	InstanceData.NPC->StartActivity(ToCharacterActivity(InstanceData.Activity));
	return EStateTreeRunStatus::Running;
}

void FZSTTask_Activity::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.OnActivityStoppedConnection.Disconnect();
	InstanceData.OnActivityEndingConnection.Disconnect();
}

#if WITH_EDITOR
FText FZSTTask_Activity::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_ENUM_DISPLAY_TEXT(EZCharacterActivityBP, Activity);
}
#endif
