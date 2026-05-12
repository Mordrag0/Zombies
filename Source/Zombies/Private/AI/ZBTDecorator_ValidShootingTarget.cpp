// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTDecorator_ValidShootingTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/ZCharacter.h"

UZBTDecorator_ValidShootingTarget::UZBTDecorator_ValidShootingTarget()
{
	NodeName = "ValidShootingTarget";
}

bool UZBTDecorator_ValidShootingTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BBComp = GetBlackboard(OwnerComp);
	AZCharacter* Target = Cast<AZCharacter>(BBComp->GetValueAsObject(BlackboardKey.SelectedKeyName));
	return Target && !Target->GetDead();
}

