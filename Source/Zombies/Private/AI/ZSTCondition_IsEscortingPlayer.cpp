// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_IsEscortingPlayer.h"
#include "StateTreeExecutionContext.h"
#include "AI/ZNPCAIController.h"


bool FZSTCondition_IsEscortingPlayer::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Controller)
	{
		return false;
	}
	const FZPathData& PathData = InstanceData.Controller->GetPathData();
	const bool bResult = (PathData.IsValid() && PathData.bEscortPlayer);
	return bResult ^ bInvert;
}

