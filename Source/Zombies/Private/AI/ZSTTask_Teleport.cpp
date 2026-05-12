// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_Teleport.h"
#include "Characters/ZNPCharacter.h"
#include "StateTreeExecutionContext.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "Movement/ZCharacterMoverComponent.h"
#include "Movement/ZNavMoverComponent.h"

EStateTreeRunStatus FZSTTask_Teleport::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character)
	{
		return EStateTreeRunStatus::Failed;
	}
	InstanceData.Character->GetNavMoverComponent()->StopMovementImmediately();
	
	if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Context.GetWorld()))
	{
		FNavLocation OutLocation;
		if (NavSys->ProjectPointToNavigation(InstanceData.TargetLocation, OutLocation))
		{
			// Offset by capsule half height so character sits on the ground
			UCapsuleComponent* Capsule = InstanceData.Character->GetCapsuleComponent();
			FVector AdjustedLocation = OutLocation.Location;
			AdjustedLocation.Z += Capsule->GetScaledCapsuleHalfHeight();
			InstanceData.Character->GetMoverComponent()->Teleport(AdjustedLocation);
			return EStateTreeRunStatus::Running;
			// if (InstanceData.Character->SetActorLocation(AdjustedLocation, true, nullptr, ETeleportType::TeleportPhysics))
			// {
			// 	// #ZTODOMOVER - FFloorCheckResult
			// 	// UCharacterMovementComponent* CharacterMovement = InstanceData.Character->GetCharacterMovement();
			// 	// CharacterMovement->FindFloor(InstanceData.Character->GetActorLocation(), CharacterMovement->CurrentFloor, false);
			// 	// CharacterMovement->AdjustFloorHeight();
			// }
		}
	}
	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR
FText FZSTTask_Teleport::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_VECTOR_DISPLAY_TEXT(TargetLocation);
}
#endif
