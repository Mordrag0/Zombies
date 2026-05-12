// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "ZBTCompositeNode_TryAll.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTCompositeNode_TryAll : public UBTCompositeNode
{
	GENERATED_BODY()
public:
	UZBTCompositeNode_TryAll();

	virtual int32 GetNextChildHandler(struct FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;

#if WITH_EDITOR
	virtual bool CanAbortLowerPriority() const override;
#endif
};

