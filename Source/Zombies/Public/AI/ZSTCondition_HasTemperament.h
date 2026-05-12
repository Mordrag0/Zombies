// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTConditionBase.h"
#include "Characters/ZNPCharacter.h"
#include "ZSTCondition_HasTemperament.generated.h"

class AZNPCharacter;
enum class EZTemperament : uint8;

USTRUCT()
struct FZSTCondition_HasTemperamentInstanceData
{
	GENERATED_BODY()
	
    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<AZNPCharacter> NPC = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	EZTemperament Temperament = static_cast<EZTemperament>(0);
};

USTRUCT(meta = (DisplayName = "Has temperament?"))
struct FZSTCondition_HasTemperament : public FZSTInvertableCondition
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTCondition_HasTemperamentInstanceData;

	FZSTCondition_HasTemperament() {}

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

