// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_RotateToFace.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "Characters/ZAICharacter.h"
#include "AI/ZNPCAIController.h"


FZSTTask_RotateToFace::FZSTTask_RotateToFace()
{
	bShouldCallTick = true;
}

EStateTreeRunStatus FZSTTask_RotateToFace::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.TargetActor.IsValid())
	{
		if (bTrackTargetActor)
		{
			InstanceData.Character->SetFocusActor(InstanceData.TargetActor.Get());
			return EStateTreeRunStatus::Running;
		}
		InstanceData.TargetDirection = (InstanceData.TargetActor->GetActorLocation() - InstanceData.Character->GetActorLocation()).GetSafeNormal();
	}
	else if (FAISystem::IsValidLocation(InstanceData.TargetLocation))
	{
		InstanceData.TargetDirection = (InstanceData.TargetLocation - InstanceData.Character->GetActorLocation()).GetSafeNormal();
	}
	else if (FAISystem::IsValidOrientation(InstanceData.TargetOrientation))
	{
		InstanceData.TargetDirection = InstanceData.TargetOrientation.Vector();
	}
	else if (FAISystem::IsValidRotation(InstanceData.TargetRotation))
	{
		InstanceData.TargetDirection = InstanceData.TargetRotation.Vector();
	}
	if (!FAISystem::IsValidLocation(InstanceData.TargetDirection))
	{
		return EStateTreeRunStatus::Failed;
	}
	if (HasFinished(Context))
	{
		return EStateTreeRunStatus::Succeeded;
	}
	InstanceData.Character->SetTargetDirection(InstanceData.TargetDirection);
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FZSTTask_RotateToFace::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (bTrackTargetActor)
	{
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		if (!InstanceData.TargetActor.IsValid())
		{
			return EStateTreeRunStatus::Failed;
		}
	}
	if (!bTrackTargetActor && HasFinished(Context))
	{
		return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
}

void FZSTTask_RotateToFace::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (bTrackTargetActor)
	{
		if (InstanceData.TargetActor.Get() == InstanceData.Character->GetFocusActor())
		{
			InstanceData.Character->ClearFocusActor();
		}
	}
	else
	{
		if (bSnapToTarget)
		{
			const FRotator TargetRotation = InstanceData.TargetDirection.Rotation();
			InstanceData.Character->SetActorRotation(FRotator(0.f, TargetRotation.Yaw, 0.f));
		}
		if (InstanceData.TargetDirection == InstanceData.Character->GetTargetDirection())
		{
			InstanceData.Character->ClearTargetDirection();
		}
	}
}

#if WITH_EDITOR
FText FZSTTask_RotateToFace::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	FText Result = GET_WEAK_OBJECT_DISPLAY_TEXT(TargetActor);
	if (Result.IsEmpty())
	{
		Result = GET_VECTOR_DISPLAY_TEXT(TargetLocation);
	}
	if (Result.IsEmpty())
	{
		Result = GET_ORIENTATION_DISPLAY_TEXT(TargetOrientation);
	}
	if (Result.IsEmpty())
	{
		Result = GET_ROTATION_DISPLAY_TEXT(TargetRotation);
	}
	if (Result.IsEmpty())
	{
		Result = INVTEXT("Invalid");
	}
	return Result;
}
#endif

bool FZSTTask_RotateToFace::HasFinished(const FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	FVector Target = InstanceData.TargetDirection;
	Target.Z = 0;
	Target.Normalize();
    const FVector Forward = InstanceData.Character->GetActorForwardVector();
    const float Dot = FVector::DotProduct(Forward, Target);
    
    // Check if facing target within tolerance
    return (Dot >= FMath::Cos(FMath::DegreesToRadians(AngleTolerance)));
}

