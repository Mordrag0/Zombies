// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_HasValidPath.h"
#include "StateTreeExecutionContext.h"
#include "AI/ZNPCAIController.h"

bool FZSTCondition_HasValidPath::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Controller)
	{
		return false;
	}
	const bool bResult = InstanceData.Controller->GetPathData().IsValid();
	return bResult ^ bInvert;
}

