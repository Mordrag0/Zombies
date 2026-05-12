// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "AITypes.h"
#include "ZSTTask_RotateToFace.generated.h"

class AZNPCAIController;
class AZAICharacter;

USTRUCT()
struct FZSTTask_RotateToFaceInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AZAICharacter> Character = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	TWeakObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FVector TargetLocation = FAISystem::InvalidLocation;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FQuat TargetOrientation = FAISystem::InvalidOrientation;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FRotator TargetRotation = FAISystem::InvalidRotation;

	FVector TargetDirection = FAISystem::InvalidDirection;
};

USTRUCT(meta = (DisplayName = "Rotate to face", Category = "AI|Action"))
struct FZSTTask_RotateToFace : public FZStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_RotateToFaceInstanceData;

	FZSTTask_RotateToFace();

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	STATETREE_NODE_DESCRIPTION()

	UPROPERTY(EditDefaultsOnly)
	float AngleTolerance = 10.f;

	UPROPERTY(EditDefaultsOnly)
	bool bSnapToTarget = false;

	UPROPERTY(EditDefaultsOnly)
	bool bTrackTargetActor = false;

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const;
#endif

protected:
	bool HasFinished(const FStateTreeExecutionContext& Context) const;
};
