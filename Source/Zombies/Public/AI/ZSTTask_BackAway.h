// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTask_MoveTo.h"
#include "ZDelegateConnection.h"
#include "ZSTTask_BackAway.generated.h"

class AZNPCAIController;
class AZNPCharacter;
struct FStateTreeWeakExecutionContext;

USTRUCT()
struct FZSTTask_BackAwayInstanceData : public FZSTTask_MoveToInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Input")
	TObjectPtr<AActor> BackAwayTargetActor = nullptr;

	FZDelegateConnection OnMoveTaskFinishedConnection;
};

USTRUCT(meta = (DisplayName = "Back away", Category = "AI|Action"))
struct FZSTTask_BackAway : public FZSTTask_MoveTo
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_BackAwayInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	
	virtual bool Link(FStateTreeLinker& Linker) override;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
	EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
	
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (NoBinding))
	float BackAwayLocationDistance = 500.f;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (NoBinding))
	float BackAwayLocationSearchRadius = 300.f;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (NoBinding))
	float TargetDistance = 100.f; // Should be smaller than BackAwayLocationDistance

protected:
	FVector ChooseBackAwayLocation(const FStateTreeExecutionContext& Context) const;
};
