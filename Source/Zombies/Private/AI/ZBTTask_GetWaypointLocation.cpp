// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_GetWaypointLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/ZWaypoint.h"
#include "AI/ZPath.h"
#include "Characters/ZNPCharacter.h"
#include "AI/ZBlackboardKeyType_GameplayTag.h"
#include "ZGameState.h"

UZBTTask_GetWaypointLocation::UZBTTask_GetWaypointLocation()
{
	NodeName = "GetWaypointLocation";

	AddBlackboardFilter<UZBlackboardKeyType_GameplayTag>(PathKey, GET_MEMBER_NAME_CHECKED(ThisClass, PathKey));
	WaypointIndexKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, WaypointIndexKey));
	MoveToKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, MoveToKey));
}

void UZBTTask_GetWaypointLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		PathKey.ResolveSelectedKey(*BBAsset);
		WaypointIndexKey.ResolveSelectedKey(*BBAsset);
		MoveToKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		PathKey.InvalidateResolvedKey();
		WaypointIndexKey.InvalidateResolvedKey();
		MoveToKey.InvalidateResolvedKey();
	}
}

EBTNodeResult::Type UZBTTask_GetWaypointLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = GetBlackboard(OwnerComp);
	const FGameplayTag PathTag = BBComp->GetValue<UZBlackboardKeyType_GameplayTag>(PathKey.SelectedKeyName);
	if (!PathTag.IsValid())
	{
		return EBTNodeResult::Failed;
	}
	const AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
	const AZPath* Path = GS ? GS->GetPath(PathTag) : nullptr;
	if (!Path)
	{
		return EBTNodeResult::Failed;
	}
	const int32 WaypointIndex = BBComp->GetValueAsInt(WaypointIndexKey.SelectedKeyName);
	const TArray<AZWaypoint*>& Waypoints = Path->GetWaypoints();
	if (!Waypoints.IsValidIndex(WaypointIndex))
	{
		return EBTNodeResult::Failed;
	}
	BBComp->SetValueAsVector(MoveToKey.SelectedKeyName, Waypoints[WaypointIndex]->GetActorLocation());
	return EBTNodeResult::Succeeded;
}

#if WITH_EDITOR
FString UZBTTask_GetWaypointLocation::GetErrorMessage() const
{
	if (!GetBlackboardAsset())
	{
		return UE::BehaviorTree::Messages::BlackboardNotSet.ToString();
	}
	return Super::GetErrorMessage();
}
#endif

