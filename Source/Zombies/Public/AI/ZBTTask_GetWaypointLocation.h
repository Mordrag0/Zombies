// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTTask_Base.h"
#include "ZBTTask_GetWaypointLocation.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTTask_GetWaypointLocation : public UZBTTask_Base
{
	GENERATED_BODY()
	
public:
	UZBTTask_GetWaypointLocation();

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
#if WITH_EDITOR
	virtual FString GetErrorMessage() const override;
#endif

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PathKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector WaypointIndexKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MoveToKey;
};

