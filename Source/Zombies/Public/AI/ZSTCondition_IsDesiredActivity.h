// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTConditionBase.h"
#include "ZSTCondition_IsDesiredActivity.generated.h"

class AZNPCAIController;
enum class EZCharacterActivityBP : uint8;

USTRUCT()
struct FZSTCondition_IsDesiredActivityInstanceData
{
	GENERATED_BODY()
	
	FZSTCondition_IsDesiredActivityInstanceData();

    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<AZNPCAIController> Controller;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	EZCharacterActivityBP Activity;
};

USTRUCT(meta = (DisplayName = "Is desired activity?"))
struct FZSTCondition_IsDesiredActivity : public FZSTInvertableCondition
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTCondition_IsDesiredActivityInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	
	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

