// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_PickBackAwayLocation.h"
#include "Characters/ZNPCharacter.h"
#include "StateTreeExecutionContext.h"
#include "NavigationSystem.h"


EStateTreeRunStatus FZSTTask_PickBackAwayLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character || !InstanceData.Target)
	{
		return EStateTreeRunStatus::Failed;
	}
	const FVector CharacterLocation = InstanceData.Character->GetActorLocation();
	const FVector TargetLocation = InstanceData.Target->GetActorLocation();
	const FVector Direction = (CharacterLocation - TargetLocation).GetSafeNormal();
	const FVector Center = CharacterLocation + Direction * InstanceData.DistanceFromTarget;

	if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Context.GetWorld()))
	{
		FNavLocation OutLocation;
		if (NavSys->GetRandomReachablePointInRadius(Center, InstanceData.SearchRadius, OutLocation))
		{
			InstanceData.BackAwayLocation = OutLocation.Location;
			return EStateTreeRunStatus::Running;
		}
	}
	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR
FText FZSTTask_PickBackAwayLocation::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_OBJECT_DISPLAY_TEXT(Target);
}
#endif

