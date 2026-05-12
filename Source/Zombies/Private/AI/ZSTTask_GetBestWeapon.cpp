// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_GetBestWeapon.h"
#include "StateTreeExecutionContext.h"
#include "Inventory/ZInventoryItemData.h"
#include "Characters/ZNPCharacter.h"
#include "Inventory/ZInventoryItem.h"
#include "Weapons/ZWeapon.h"


FZSTTask_GetBestWeapon::FZSTTask_GetBestWeapon()
{
	bShouldStateChangeOnReselect = false;
}

EStateTreeRunStatus FZSTTask_GetBestWeapon::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character)
	{
		return EStateTreeRunStatus::Failed;
	}
	const UZInventoryItemData* InventoryItemData = InstanceData.Character->PickBestWeapon();
	if (!InventoryItemData || !InventoryItemData->ItemClass) // #ZTODO what if it's not loaded?
	{
		return EStateTreeRunStatus::Failed;
	}
	InstanceData.WeaponClass = InventoryItemData->ItemClass.Get();
	return InstanceData.WeaponClass ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Failed;
}


