// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_GetDesiredActivity.h"
#include "StateTreeExecutionContext.h"
#include "AI/ZNPCAIController.h"
#include "ZTypes.h"
#include "ZGameState.h"

FZSTTask_GetDesiredActivityInstanceData::FZSTTask_GetDesiredActivityInstanceData()
{
	Activity = EZCharacterActivityBP::None;
}

EStateTreeRunStatus FZSTTask_GetDesiredActivity::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Controller)
	{
		return EStateTreeRunStatus::Failed;
	}
	AZGameState* GS = Context.GetWorld()->GetGameState<AZGameState>();
	if (!GS)
	{
		return EStateTreeRunStatus::Failed;
	}
	const EZCharacterActivity DesiredActivity = InstanceData.Controller->GetCachedDesiredActivity();
	InstanceData.Activity = ToCharacterActivityBP(DesiredActivity);
	InstanceData.bInteractionActivity = IsInteractionActivity(DesiredActivity);
	return (InstanceData.Activity != EZCharacterActivityBP::None) ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Failed;
}


