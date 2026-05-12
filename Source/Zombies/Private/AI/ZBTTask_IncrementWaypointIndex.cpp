// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_IncrementWaypointIndex.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/ZWaypoint.h"
#include "AI/ZPath.h"
#include "Characters/ZNPCharacter.h"
#include "AI/ZBlackboardKeyType_GameplayTag.h"
#include "ZGameState.h"
#include "Characters/ZFPCharacter.h"

UZBTTask_IncrementWaypointIndex::UZBTTask_IncrementWaypointIndex()
{
	NodeName = "IncrementWaypointIndex";
	
	AddBlackboardFilter<UZBlackboardKeyType_GameplayTag>(PathKey, GET_MEMBER_NAME_CHECKED(ThisClass, PathKey));
	WaypointIndexKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, WaypointIndexKey));
	EscortPlayerKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, EscortPlayerKey), AZFPCharacter::StaticClass());
}

void UZBTTask_IncrementWaypointIndex::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		PathKey.ResolveSelectedKey(*BBAsset);
		WaypointIndexKey.ResolveSelectedKey(*BBAsset);
		EscortPlayerKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		PathKey.InvalidateResolvedKey();
		WaypointIndexKey.InvalidateResolvedKey();
		EscortPlayerKey.InvalidateResolvedKey();
	}
}

EBTNodeResult::Type UZBTTask_IncrementWaypointIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = GetBlackboard(OwnerComp);
	const FGameplayTag PathTag = BBComp->GetValue<UZBlackboardKeyType_GameplayTag>(PathKey.SelectedKeyName);
	if (!PathTag.IsValid())
	{
		return EBTNodeResult::Failed;
	}
	const int32 WaypointIndex = BBComp->GetValueAsInt(WaypointIndexKey.SelectedKeyName);
	AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
	const AZPath* Path = GS ? GS->GetPath(PathTag) : nullptr;
	if (!GS || !Path)
	{
		return EBTNodeResult::Failed;
	}
	const TArray<AZWaypoint*>& Waypoints = Path->GetWaypoints();
	if (!Waypoints.IsValidIndex(WaypointIndex + 1))
	{
		if ((WaypointIndex + 1) >= Path->GetWaypoints().Num())
		{
			AZFPCharacter* EscortPlayer = Cast<AZFPCharacter>(BBComp->GetValueAsObject(EscortPlayerKey.SelectedKeyName));
			AController* EventInstigator = EscortPlayer ? EscortPlayer->GetController() : nullptr;
			GS->CompletePath(Path, GetNPCharacter(OwnerComp), EventInstigator);
		}
		return EBTNodeResult::Failed; // Path completed, abort the path following sequence
	}
	BBComp->SetValueAsInt(WaypointIndexKey.SelectedKeyName, WaypointIndex + 1);
	return EBTNodeResult::Succeeded;
}


#if WITH_EDITOR
FString UZBTTask_IncrementWaypointIndex::GetErrorMessage() const
{
	if (!GetBlackboardAsset())
	{
		return UE::BehaviorTree::Messages::BlackboardNotSet.ToString();
	}
	return Super::GetErrorMessage();
}
#endif

