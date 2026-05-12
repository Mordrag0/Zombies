// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_IsAtLocation.h"
#include "Characters/ZNPCharacter.h"
#include "StateTreeExecutionContext.h"


bool FZSTCondition_IsAtLocation::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character)
	{
		return false;
	}
	const bool bResult = (FVector::DistSquared(InstanceData.Character->GetActorLocation(), InstanceData.Location) < FMath::Square(DistanceTolerance))
		&& !bCheckRotation || (FMath::FindDeltaAngleDegrees(InstanceData.Character->GetActorRotation().Yaw, InstanceData.Rotation.Yaw) < AngleTolerance);
	return bResult ^ bInvert;
}

#if WITH_EDITOR
FText FZSTCondition_IsAtLocation::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	if (bCheckRotation)
	{
		return JoinParameters({ GET_VECTOR_DISPLAY_TEXT(Location), GET_ROTATION_DISPLAY_TEXT(Rotation) });
	}
	else
	{
		return GET_VECTOR_DISPLAY_TEXT(Location);
	}
}
#endif
