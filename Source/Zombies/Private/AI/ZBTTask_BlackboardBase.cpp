// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_BlackboardBase.h"

UZBTTask_BlackboardBase::UZBTTask_BlackboardBase()
{
	NodeName = "ZBlackboardBase";
}

void UZBTTask_BlackboardBase::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		BlackboardKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		BlackboardKey.InvalidateResolvedKey();
	}
}

FName UZBTTask_BlackboardBase::GetSelectedBlackboardKey() const
{
	return BlackboardKey.SelectedKeyName;
}

#if WITH_EDITOR
FString UZBTTask_BlackboardBase::GetErrorMessage() const
{
	if (!GetBlackboardAsset())
	{
		return UE::BehaviorTree::Messages::BlackboardNotSet.ToString();
	}
	return Super::GetErrorMessage();
}
#endif

