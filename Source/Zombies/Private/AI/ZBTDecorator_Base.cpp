// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTDecorator_Base.h"
#include "AI/ZAIController.h"
#include "Characters/ZCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/ZNPCAIController.h"

AZAIController* UZBTDecorator_Base::GetAIController(UBehaviorTreeComponent& OwnerComp) const
{
	return Cast<AZAIController>(OwnerComp.GetAIOwner());
}

AZNPCAIController* UZBTDecorator_Base::GetNPCAIController(UBehaviorTreeComponent& OwnerComp) const
{
	return Cast<AZNPCAIController>(OwnerComp.GetAIOwner());
}

AZCharacter* UZBTDecorator_Base::GetCharacter(UBehaviorTreeComponent& OwnerComp) const
{
	if (AZAIController* AIController = GetAIController(OwnerComp))
	{
		return AIController->GetZCharacter();
	}
	return nullptr;
}

AZNPCharacter* UZBTDecorator_Base::GetNPCharacter(UBehaviorTreeComponent& OwnerComp) const
{
	if (AZNPCAIController* AIController = GetNPCAIController(OwnerComp))
	{
		return AIController->GetNPCharacter();
	}
	return nullptr;
}

UBlackboardComponent* UZBTDecorator_Base::GetBlackboard(UBehaviorTreeComponent& OwnerComp) const
{
	if (AZAIController* AIController = GetAIController(OwnerComp))
	{
		return AIController->GetBlackboardComponent();
	}
	return nullptr;
}

