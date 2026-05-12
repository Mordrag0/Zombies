// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTDecorator_IsTime.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UZBTDecorator_IsTime::UZBTDecorator_IsTime()
{
	NodeName = "IsTime";
}

bool UZBTDecorator_IsTime::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BBComp = GetBlackboard(OwnerComp);
	const int32 Time = BBComp->GetValueAsInt(BlackboardKey.SelectedKeyName);
	if (TimeTo < TimeFrom)
	{
		return (Time >= TimeFrom) || (Time < TimeTo);
	}
	else
	{
		return (Time >= TimeFrom) && (Time < TimeTo);
	}
}

