// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_WaitForPlayer.h"
#include "StateTreeExecutionContext.h"
#include "Characters/ZNPCharacter.h"
#include "Characters/ZFPCharacter.h"


FZSTTask_WaitForPlayer::FZSTTask_WaitForPlayer()
{
	bShouldCallTick = true;
}

EStateTreeRunStatus FZSTTask_WaitForPlayer::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character || !InstanceData.Player)
	{
		return EStateTreeRunStatus::Failed;
	}
	return IsPlayerNear(Context) ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FZSTTask_WaitForPlayer::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character || !InstanceData.Player)
	{
		return EStateTreeRunStatus::Failed;
	}
	return IsPlayerNear(Context) ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FZSTTask_WaitForPlayer::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_OBJECT_DISPLAY_TEXT(Player);
}
#endif

bool FZSTTask_WaitForPlayer::IsPlayerNear(const FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	return (FVector::DistSquared(InstanceData.Character->GetActorLocation(), InstanceData.Player->GetActorLocation()) < FMath::Square(InstanceData.Distance));
}


