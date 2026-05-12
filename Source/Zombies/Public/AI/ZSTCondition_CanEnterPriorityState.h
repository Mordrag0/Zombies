// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTConditionBase.h"
#include "ZSTCondition_CanEnterPriorityState.generated.h"

class AZNPCAIController;
enum class EZNPCStatePriority : uint8;

USTRUCT()
struct FZSTCondition_CanEnterPriorityStateInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Context")
	TObjectPtr<AZNPCAIController> Controller = nullptr;
};

USTRUCT(meta = (DisplayName = "Can enter priority state?"))
struct FZSTCondition_CanEnterPriorityState : public FZSTConditionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTCondition_CanEnterPriorityStateInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	STATETREE_NODE_DESCRIPTION()
	
#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif

	UPROPERTY(EditAnywhere, Category = "Parameter")
	EZNPCStatePriority StatePriority = static_cast<EZNPCStatePriority>(0);
};




