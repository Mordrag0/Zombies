// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_HasItemEquipped.h"
#include "StateTreeExecutionContext.h"
#include "Inventory/ZEquippableItem.h"
#include "Characters/ZNPCharacter.h"
#include "Components/ZEquipmentComponent.h"


bool FZSTCondition_HasItemEquipped::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.NPC || !InstanceData.ItemClass)
	{
		return false;
	}
	AZEquippableItem* EquippedItem = InstanceData.NPC->GetEquipmentComponent()->GetEquippedItem();
	return (EquippedItem && EquippedItem->IsA(InstanceData.ItemClass) && EquippedItem->IsIdle());
}

#if WITH_EDITOR
FText FZSTCondition_HasItemEquipped::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_OBJECT_DISPLAY_TEXT(ItemClass);
}
#endif

