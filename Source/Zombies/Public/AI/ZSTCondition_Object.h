// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTConditionBase.h"
#include "ZSTCondition_Object.generated.h"

USTRUCT()
struct FZSTCondition_IsObjectValidInstanceData
{
	GENERATED_BODY()
	
    UPROPERTY(VisibleAnywhere, Category = "Input")
    TObjectPtr<UObject> Object = nullptr;
};

USTRUCT(meta = (DisplayName = "Is object valid?"), Category = "ZStateTree")
struct FZSTCondition_IsObjectValid : public FZSTInvertableCondition
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTCondition_IsObjectValidInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	
	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

