// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTConditionBase.h"
#include "ZSTCondition_IsTime.generated.h"

USTRUCT()
struct FZSTCondition_IsTimeInstanceData
{
	GENERATED_BODY()
	
    UPROPERTY(VisibleAnywhere, Category = "Input")
    int32 NewHour = 0;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	int32 Hour = 0;
};

USTRUCT(meta = (DisplayName = "Is time?"))
struct FZSTCondition_IsTime : public FZSTConditionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTCondition_IsTimeInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	
	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};


