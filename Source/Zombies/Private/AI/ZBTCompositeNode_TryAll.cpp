// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTCompositeNode_TryAll.h"

UZBTCompositeNode_TryAll::UZBTCompositeNode_TryAll()
{
	NodeName = "Try all";
}

int32 UZBTCompositeNode_TryAll::GetNextChildHandler(struct FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	// always move to next child regardless of result
	int32 NextChildIdx = BTSpecialChild::ReturnToParent;

	if (PrevChild == BTSpecialChild::NotInitialized)
	{
		// newly activated: start from first
		NextChildIdx = 0;
	}
	else if ((PrevChild + 1) < GetChildrenNum())
	{
		// success = choose next child
		NextChildIdx = PrevChild + 1;
	}

	return NextChildIdx;
}

#if WITH_EDITOR
bool UZBTCompositeNode_TryAll::CanAbortLowerPriority() const
{
	return false;
}
#endif

