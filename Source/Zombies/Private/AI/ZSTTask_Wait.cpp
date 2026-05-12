// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_Wait.h"


EStateTreeRunStatus FZSTTask_Wait::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return EStateTreeRunStatus::Running;
}

