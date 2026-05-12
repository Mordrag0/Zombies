// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZSTTask_PickBackAwayLocation.generated.h"

class AZNPCharacter;
class AZCharacter;

USTRUCT()
struct FZSTTask_PickBackAwayLocationInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AZNPCharacter> Character = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	TObjectPtr<AActor> Target;

	UPROPERTY(VisibleAnywhere, Category = "Output")
	FVector BackAwayLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float DistanceFromTarget = 500.f;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float SearchRadius = 300.f;
};

USTRUCT(meta = (DisplayName = "Pick back away location", Category = "AI"))
struct FZSTTask_PickBackAwayLocation : public FZStateTreeAITaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_PickBackAwayLocationInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

