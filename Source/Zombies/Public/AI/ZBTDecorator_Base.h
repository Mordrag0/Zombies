// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "ZBTDecorator_Base.generated.h"

class AZAIController;
class AZCharacter;
class UBlackboardComponent;
class AZNPCAIController;
class AZNPCharacter;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTDecorator_Base : public UBTDecorator
{
	GENERATED_BODY()
	
protected:
    AZAIController* GetAIController(UBehaviorTreeComponent& OwnerComp) const;

    AZNPCAIController* GetNPCAIController(UBehaviorTreeComponent& OwnerComp) const;

    AZCharacter* GetCharacter(UBehaviorTreeComponent& OwnerComp) const;

    AZNPCharacter* GetNPCharacter(UBehaviorTreeComponent& OwnerComp) const;

    UBlackboardComponent* GetBlackboard(UBehaviorTreeComponent& OwnerComp) const;
};

