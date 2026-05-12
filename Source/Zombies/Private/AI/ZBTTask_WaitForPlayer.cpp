// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_WaitForPlayer.h"
#include "Characters/ZFPCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/ZNPCharacter.h"

UZBTTask_WaitForPlayer::UZBTTask_WaitForPlayer()
{
	NodeName = "WaitForPlayer";

	INIT_TASK_NODE_NOTIFY_FLAGS();
	bCreateNodeInstance = true;

	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, BlackboardKey), AZFPCharacter::StaticClass());
}

EBTNodeResult::Type UZBTTask_WaitForPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UZBTTask_WaitForPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBComp = GetBlackboard(OwnerComp);
	const AZFPCharacter* EscortedCharacter = Cast<AZFPCharacter>(BBComp->GetValueAsObject(BlackboardKey.SelectedKeyName));
	const AZNPCharacter* NPCharacter = GetNPCharacter(OwnerComp);
	if (!EscortedCharacter || !NPCharacter)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	if (IsPlayerNearby(EscortedCharacter, NPCharacter))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	SetNextTickTime(NodeMemory, CheckInterval);
}

bool UZBTTask_WaitForPlayer::IsPlayerNearby(const AZFPCharacter* Player, const AZNPCharacter* CharacterOwner) const
{
	const float DistSquared = FVector::DistSquared(Player->GetActorLocation(), CharacterOwner->GetActorLocation());
	return DistSquared < FMath::Square(WaitRadius);
}

