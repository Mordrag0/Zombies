// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_GetPivot.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ZInteractable.h"

UZBTTask_GetPivot::UZBTTask_GetPivot()
{
	NodeName = "Get interactable pivot";

	LocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, LocationKey));
	RotationKey.AddRotatorFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, RotationKey));
	ObjectKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, ObjectKey), AActor::StaticClass());
}

EBTNodeResult::Type UZBTTask_GetPivot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = GetBlackboard(OwnerComp);
	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(ObjectKey.SelectedKeyName));
	if (!Target || !Target->Implements<UZInteractable>())
	{
		return EBTNodeResult::Failed;
	}
	IZInteractable* Interactable = Cast<IZInteractable>(Target);
	const FTransform Pivot = Interactable->GetPivotPoint();
	Blackboard->SetValueAsVector(LocationKey.SelectedKeyName, Pivot.GetLocation());
	Blackboard->SetValueAsRotator(RotationKey.SelectedKeyName, Pivot.GetRotation().Rotator());
	return EBTNodeResult::Succeeded;
}

void UZBTTask_GetPivot::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		LocationKey.ResolveSelectedKey(*BBAsset);
		RotationKey.ResolveSelectedKey(*BBAsset);
		ObjectKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		LocationKey.InvalidateResolvedKey();
		RotationKey.InvalidateResolvedKey();
		ObjectKey.InvalidateResolvedKey();
	}
}

#if WITH_EDITOR
FString UZBTTask_GetPivot::GetErrorMessage() const
{
	if (GetBlackboardAsset() == nullptr)
	{
		return UE::BehaviorTree::Messages::BlackboardNotSet.ToString();
	}
	return Super::GetErrorMessage();
}
#endif

