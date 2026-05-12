// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_MoveTo.h"
#include "AIController.h"
#include "Tasks/AITask_MoveTo.h"
#include "Characters/ZCharacter.h"
#include "StateTreeExecutionContext.h"
#include "ZTypes.h"
#include "AITypes.h"
#include "VisualLogger/VisualLogger.h"
#include "AI/ZSTHelpers.h"
#include "Characters/ZNPCharacter.h"
#include "Movement/ZNavMoverComponent.h"

FZSTTask_MoveToInstanceData::FZSTTask_MoveToInstanceData()
{
	bTrackMovingGoal = false; // By default, we want this off
	DestinationMoveTolerance = 1.f;
	GaitState = EZGaitState::Walk;
}

EStateTreeRunStatus FZSTTask_MoveTo::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character)
	{
		return EStateTreeRunStatus::Failed;
	}
	InstanceData.Character->SetMovementState(InstanceData.GaitState);
	InstanceData.LastDestination = InstanceData.Destination; // Prevent first tick from performing an unnecessary move task
	return Super::EnterState(Context, Transition);
}

void FZSTTask_MoveTo::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FStateTreeMoveToTask::ExitState(Context, Transition);
	
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.Character->GetNavMoverComponent()->StopMovementImmediately();
}

EStateTreeRunStatus FZSTTask_MoveTo::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	// Skip Super call
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.MoveToTask)
	{
		if (InstanceData.bTrackMovingGoal && !InstanceData.TargetActor)
		{
			const FVector CurrentDestination = InstanceData.MoveToTask->GetMoveRequestRef().GetDestination();
			if (InstanceData.Destination != InstanceData.LastDestination) // Destination updated
			{
				InstanceData.LastDestination = InstanceData.Destination;
				if (FVector::DistSquared(CurrentDestination, InstanceData.Destination) > (InstanceData.DestinationMoveTolerance * InstanceData.DestinationMoveTolerance))
				{
					UE_VLOG(Context.GetOwner(), LogStateTree, Log, TEXT("FStateTreeMoveToTask destination has moved enough. Restarting task."));
					return PerformMoveTask(Context, *InstanceData.AIController);
				}
			}
		}
		return EStateTreeRunStatus::Running;
	}
	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR
FText FZSTTask_MoveTo::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	FText TargetValue = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetValue.IsEmpty())
	{
		TargetValue = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Destination)), Formatting);
	}
	const FText GaitText = GET_ENUM_DISPLAY_TEXT(EZGaitState, GaitState);
	const FString FormatStr = TEXT("{0} <b>to</> {1}");
	const FText FormatText = (Formatting == EStateTreeNodeFormatting::RichText)
		? FText::AsCultureInvariant(FormatStr)
		: FText::AsCultureInvariant(StripRichTextTags(FormatStr));
	return FText::Format(FormatText, GaitText, TargetValue);
}
#endif


