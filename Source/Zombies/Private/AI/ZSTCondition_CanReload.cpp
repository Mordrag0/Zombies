// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_CanReload.h"
#include "Characters/ZNPCharacter.h"
#include "StateTreeExecutionContext.h"
#include "Weapons/ZRangedWeapon.h"
#include "Components/ZEquipmentComponent.h"
#include "Inventory/ZInventoryComponent.h"


bool FZSTCondition_CanReload::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.NPC)
	{
		return false;
	}
	AZRangedWeapon* RangedWeapon = Cast<AZRangedWeapon>(InstanceData.NPC->GetEquipmentComponent()->GetEquippedItem());
	if (!RangedWeapon)
	{
		return false;
	}
	const bool bHasAmmoInInventory = (InstanceData.NPC->GetInventoryComponent()->GetItemCount(RangedWeapon->GetAmmoType()) > 0);
	const bool bCanWeaponReload = (RangedWeapon->GetAmmoInMag() < RangedWeapon->GetAmmoPerMag());
	const bool bResult = bHasAmmoInInventory && bCanWeaponReload;
	return bResult ^ bInvert;
}

