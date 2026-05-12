// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_HasCombatTarget.h"
#include "Characters/ZCharacter.h"
#include "StateTreeExecutionContext.h"
#include "AI/ZAIController.h"

bool FZSTCondition_HasCombatTarget::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AZCharacter* CombatTarget = InstanceData.AIController ? InstanceData.AIController->GetCombatTarget() : nullptr;
	return CombatTarget && !CombatTarget->GetDead();
}

