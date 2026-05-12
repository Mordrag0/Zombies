// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/StateTreeMoveToTask.h"
#include "AITypes.h"
#include "ZSTTask_MoveTo.generated.h"

class AZNPCharacter;
enum class EZGaitState : uint8;

USTRUCT()
struct FZSTTask_MoveToInstanceData : public FStateTreeMoveToTaskInstanceData
{
	GENERATED_BODY()

	FZSTTask_MoveToInstanceData();
	
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AZNPCharacter> Character = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	EZGaitState GaitState;

	FVector LastDestination = FAISystem::InvalidDirection;
};

USTRUCT(meta = (DisplayName = "ZMove to", Category = "AI|Action"))
struct FZSTTask_MoveTo : public FStateTreeMoveToTask
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_MoveToInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
	EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
