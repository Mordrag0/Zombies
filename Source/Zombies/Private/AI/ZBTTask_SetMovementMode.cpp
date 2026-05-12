// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_SetMovementMode.h"
#include "Characters/ZCharacter.h"

UZBTTask_SetMovementMode::UZBTTask_SetMovementMode()
{
	NodeName = "Set movement mode";
}

EBTNodeResult::Type UZBTTask_SetMovementMode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AZCharacter* Character = GetCharacter(OwnerComp);
	if (!Character)
	{
		return EBTNodeResult::Failed;
	}
	Character->SetMovementState(State);
	return EBTNodeResult::Succeeded;
}

