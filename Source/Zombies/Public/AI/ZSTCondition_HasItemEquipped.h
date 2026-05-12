// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTConditionBase.h"
#include "Inventory/ZEquippableItem.h"
#include "ZSTCondition_HasItemEquipped.generated.h"

class AZNPCharacter;

USTRUCT()
struct FZSTCondition_HasItemEquippedInstanceData
{
	GENERATED_BODY()
	
    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<AZNPCharacter> NPC;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	TSubclassOf<AZEquippableItem> ItemClass = AZEquippableItem::StaticClass();
};

USTRUCT(meta = (DisplayName = "Has item equipped?"))
struct FZSTCondition_HasItemEquipped : public FZSTConditionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTCondition_HasItemEquippedInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	
	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};
