// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_Equip.h"
#include "StateTreeExecutionContext.h"
#include "Characters/ZNPCharacter.h"
#include "Components/ZEquipmentComponent.h"
#include "Inventory/ZEquippableItem.h"
#include "StateTreeAsyncExecutionContext.h"


FZSTTask_Equip::FZSTTask_Equip()
{
	bShouldStateChangeOnReselect = false;
}

EStateTreeRunStatus FZSTTask_Equip::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character || !InstanceData.ItemClass)
	{
		return EStateTreeRunStatus::Failed;
	}
	UZEquipmentComponent* EquipmentComponent = InstanceData.Character->GetEquipmentComponent();
	AZEquippableItem* EquippedItem = EquipmentComponent->GetEquippedItem();
	if (EquippedItem && (EquippedItem->GetClass() == InstanceData.ItemClass) && !EquippedItem->IsUnequipping())
	{
		return EStateTreeRunStatus::Succeeded;
	}

	EquipmentComponent->StartEquip(InstanceData.ItemClass);
	return EStateTreeRunStatus::Running;
}

void FZSTTask_Equip::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character || !InstanceData.ItemClass)
	{
		return;
	}
	UZEquipmentComponent* EquipmentComponent = InstanceData.Character->GetEquipmentComponent();
	EquipmentComponent->StartUnequip();
}

#if WITH_EDITOR
FText FZSTTask_Equip::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_OBJECT_DISPLAY_TEXT(ItemClass);
}
#endif
