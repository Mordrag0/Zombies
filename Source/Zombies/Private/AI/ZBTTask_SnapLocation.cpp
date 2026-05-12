// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_SnapLocation.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Characters/ZCharacter.h"
#include "Movement/ZCharacterMovementComponent.h"

UZBTTask_SnapLocation::UZBTTask_SnapLocation()
{
	NodeName = "Snap location";

	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, BlackboardKey));
}

EBTNodeResult::Type UZBTTask_SnapLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = GetBlackboard(OwnerComp);
	const FVector SnapLocation = BlackBoard->GetValueAsVector(BlackboardKey.SelectedKeyName);
	AZCharacter* Character = GetCharacter(OwnerComp);
	//Character->GetZCharacterMovement()->StopMovementImmediately();
	
    // Offset by capsule half height so character sits on the ground
    UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
    FVector AdjustedLocation = SnapLocation;
    AdjustedLocation.Z += Capsule->GetScaledCapsuleHalfHeight();
    
    Character->SetActorLocation(AdjustedLocation, false, nullptr, ETeleportType::TeleportPhysics);
    return EBTNodeResult::Succeeded;
}

