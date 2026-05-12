// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTTask_Base.h"
#include "ZBTTask_GetPivot.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTTask_GetPivot : public UZBTTask_Base
{
	GENERATED_BODY()
	
public:
	UZBTTask_GetPivot();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

#if WITH_EDITOR
	virtual FString GetErrorMessage() const override;
#endif

protected:
	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector LocationKey;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector RotationKey;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector ObjectKey;
};

