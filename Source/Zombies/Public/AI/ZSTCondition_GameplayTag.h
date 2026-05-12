// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTConditionBase.h"
#include "GameplayTagContainer.h"
#include "ZSTCondition_GameplayTag.generated.h"

USTRUCT()
struct FZSTCondition_IsGameplayTagValidInstanceData
{
	GENERATED_BODY()
	
    UPROPERTY(VisibleAnywhere, Category = "Input")
    FGameplayTag Tag = FGameplayTag::EmptyTag;
};

USTRUCT(meta = (DisplayName = "Is gameplay tag valid?"), Category = "ZStateTree")
struct FZSTCondition_IsGameplayTagValid : public FZSTInvertableCondition
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTCondition_IsGameplayTagValidInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	
	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};




