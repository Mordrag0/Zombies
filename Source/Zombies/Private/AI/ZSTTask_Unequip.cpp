// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_Unequip.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeAsyncExecutionContext.h"
#include "Components/ZEquipmentComponent.h"
#include "Inventory/ZEquippableItem.h"
#include "Characters/ZNPCharacter.h"


EStateTreeRunStatus FZSTTask_Unequip::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character)
	{
		return EStateTreeRunStatus::Failed;
	}
	UZEquipmentComponent* EquipmentComponent = InstanceData.Character->GetEquipmentComponent();
	AZEquippableItem* EquippedItem = EquipmentComponent->GetEquippedItem();
	if (!EquippedItem)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	EquipmentComponent->StartUnequip();
	BIND_MULTICAST(InstanceData.OnUnequipCompleteConnection, EquipmentComponent, EquipmentComponent->OnUnequipComplete, 
		[WeakContext = Context.MakeWeakExecutionContext()](AZEquippableItem* Item)
	{
		WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
	});
	return EStateTreeRunStatus::Running;
}

