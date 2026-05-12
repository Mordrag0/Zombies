// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ZBTTask_Base.generated.h"

class AZAIController;
class AZCharacter;
class UBlackboardComponent;
class AZNPCAIController;
class AZNPCharacter;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZBTTask_Base : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UZBTTask_Base();

protected:
	AZAIController* GetAIController(UBehaviorTreeComponent& OwnerComp) const;

	AZNPCAIController* GetNPCAIController(UBehaviorTreeComponent& OwnerComp) const;

	AZCharacter* GetCharacter(UBehaviorTreeComponent& OwnerComp) const;

	AZNPCharacter* GetNPCharacter(UBehaviorTreeComponent& OwnerComp) const;

	UBlackboardComponent* GetBlackboard(UBehaviorTreeComponent& OwnerComp) const;

	template<typename T>
    static void AddBlackboardFilter(FBlackboardKeySelector& KeySelector, const FName PropertyName)
    {
        KeySelector.AllowedTypes.Add(NewObject<T>(GetTransientPackage(), MakeUniqueObjectName(
			GetTransientPackage(), T::StaticClass(), *FString::Printf(TEXT("%s_%s"), *PropertyName.ToString(), *T::StaticClass()->GetName())), 
			RF_Public));
    }
};

