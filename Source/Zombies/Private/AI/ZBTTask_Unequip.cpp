// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_Unequip.h"
#include "Characters/ZCharacter.h"
#include "Inventory/ZEquippableItem.h"
#include "Components/ZEquipmentComponent.h"

UZBTTask_Unequip::UZBTTask_Unequip()
{
	NodeName = "Unequip";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UZBTTask_Unequip::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AZCharacter* Character = GetCharacter(OwnerComp);
	if (!Character)
	{
		return EBTNodeResult::Failed;
	}
	EquipmentComponent = Character->GetEquipmentComponent();

	AZEquippableItem* Item = EquipmentComponent->GetEquippedItem();
	if (!Item)
	{
		return EBTNodeResult::Failed;
	}
	EquipmentComponent->StartUnequip();
	if (!Item->IsUnequipping())
	{
		return EBTNodeResult::Failed;
	}
	BTComp = &OwnerComp;
	OnUnequipHandle = EquipmentComponent->OnUnequipComplete.AddUObject(this, &ThisClass::OnFinished);
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UZBTTask_Unequip::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EquipmentComponent->OnUnequipComplete.Remove(OnUnequipHandle);
	return EBTNodeResult::Aborted;
}

void UZBTTask_Unequip::OnFinished(AZEquippableItem* Item)
{
	EquipmentComponent->OnUnequipComplete.Remove(OnUnequipHandle);
	FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
}

