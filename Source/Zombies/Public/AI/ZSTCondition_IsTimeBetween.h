// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTConditionBase.h"
#include "ZSTCondition_IsTimeBetween.generated.h"

USTRUCT()
struct FZSTCondition_IsTimeBetweenInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Input")
	int32 NewHour = 0;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	int32 FromHour = 0;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	int32 ToHour = 0;
};

USTRUCT(meta = (DisplayName = "Is time between?"))
struct FZSTCondition_IsTimeBetween : public FZSTInvertableCondition
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTCondition_IsTimeBetweenInstanceData;

	FZSTCondition_IsTimeBetween() {}

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const;
#endif
};


