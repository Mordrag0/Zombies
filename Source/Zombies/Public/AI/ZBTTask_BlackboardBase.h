// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTTask_Base.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "ZBTTask_BlackboardBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZBTTask_BlackboardBase : public UZBTTask_Base
{
	GENERATED_BODY()
	
public:
	UZBTTask_BlackboardBase();

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	/** get name of selected blackboard key */
	FName GetSelectedBlackboardKey() const;

protected:
#if WITH_EDITOR
	virtual FString GetErrorMessage() const override;
#endif

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BlackboardKey;
};

