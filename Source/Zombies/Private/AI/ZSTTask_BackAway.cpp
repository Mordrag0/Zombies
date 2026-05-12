// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_BackAway.h"
#include "Characters/ZCharacter.h"
#include "StateTreeExecutionContext.h"
#include "ZTypes.h"
#include "NavigationSystem.h"
#include "Characters/ZNPCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"
#include "StateTreeAsyncExecutionContext.h"
#include "Tasks/AITask_MoveTo.h"
#include "AI/ZSTHelpers.h"

bool FZSTTask_BackAway::Link(FStateTreeLinker& Linker)
{
	const bool bResult = Super::Link(Linker);

	bShouldCallTick = true;
	bShouldCopyBoundPropertiesOnTick = false;

	return bResult;
}

EStateTreeRunStatus FZSTTask_BackAway::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	ensure(!InstanceData.TargetActor);
	ensure(BackAwayLocationDistance > TargetDistance);
	const FVector NewDestination = ChooseBackAwayLocation(Context);
	if (!FAISystem::IsValidLocation(NewDestination))
	{
		return EStateTreeRunStatus::Failed;
	}
	InstanceData.Destination = NewDestination;
	return Super::EnterState(Context, Transition);
}

EStateTreeRunStatus FZSTTask_BackAway::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character || !InstanceData.MoveToTask)
	{
		return EStateTreeRunStatus::Failed;
	}
	const FVector CurrentDestination = InstanceData.MoveToTask->GetMoveRequestRef().GetDestination();
	if (FVector::DistSquared(InstanceData.Character->GetActorLocation(), CurrentDestination) < FMath::Square(TargetDistance))
	{
		const FVector NewDestination = ChooseBackAwayLocation(Context);
		if (!FAISystem::IsValidLocation(NewDestination))
		{
			return EStateTreeRunStatus::Failed;
		}
		InstanceData.Destination = NewDestination;
		return PerformMoveTask(Context, *InstanceData.AIController);
	}
	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FZSTTask_BackAway::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	const FText TargetText = GET_OBJECT_DISPLAY_TEXT(TargetActor);
	const FText GaitText = GET_ENUM_DISPLAY_TEXT(EZGaitState, GaitState);
	const FString FormatStr = TEXT("{0} <b>away from</> {1}");
	const FText FormatText = (Formatting == EStateTreeNodeFormatting::RichText)
		? FText::AsCultureInvariant(FormatStr)
		: FText::AsCultureInvariant(StripRichTextTags(FormatStr));
	return FText::Format(FormatText, GaitText, TargetText);
}
#endif

FVector FZSTTask_BackAway::ChooseBackAwayLocation(const FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character || !InstanceData.BackAwayTargetActor)
	{
		return FAISystem::InvalidLocation;
	}
	const FVector CharacterLocation = InstanceData.Character->GetActorLocation();
	const FVector TargetLocation = InstanceData.BackAwayTargetActor->GetActorLocation();
	const FVector Direction = (CharacterLocation - TargetLocation).GetSafeNormal();
	const FVector Center = CharacterLocation + Direction * BackAwayLocationDistance;

	if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Context.GetWorld()))
	{
		for (int32 Idx = 0; Idx < 3; ++Idx)
		{
			FNavLocation OutLocation;
			if (NavSys->GetRandomReachablePointInRadius(Center, BackAwayLocationSearchRadius, OutLocation))
			{
				return OutLocation.Location;
			}
		}
	}
	return FAISystem::InvalidLocation;
}


