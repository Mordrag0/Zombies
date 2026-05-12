// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_Base.h"
#include "AI/ZAIController.h"
#include "Characters/ZCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/ZNPCAIController.h"
#include "Characters/ZNPCharacter.h"

UZBTTask_Base::UZBTTask_Base()
{
	NodeName = "ZTaskBase";
}

AZAIController* UZBTTask_Base::GetAIController(UBehaviorTreeComponent& OwnerComp) const
{
	return Cast<AZAIController>(OwnerComp.GetAIOwner());
}

AZNPCAIController* UZBTTask_Base::GetNPCAIController(UBehaviorTreeComponent& OwnerComp) const
{
	return Cast<AZNPCAIController>(OwnerComp.GetAIOwner());
}

AZCharacter* UZBTTask_Base::GetCharacter(UBehaviorTreeComponent& OwnerComp) const
{
	if (AZAIController* AIController = GetAIController(OwnerComp))
	{
		return AIController->GetZCharacter();
	}
	return nullptr;
}

AZNPCharacter* UZBTTask_Base::GetNPCharacter(UBehaviorTreeComponent& OwnerComp) const
{
	if (AZNPCAIController* AIController = GetNPCAIController(OwnerComp))
	{
		return AIController->GetNPCharacter();
	}
	return nullptr;
}

UBlackboardComponent* UZBTTask_Base::GetBlackboard(UBehaviorTreeComponent& OwnerComp) const
{
	return OwnerComp.GetBlackboardComponent();
}

