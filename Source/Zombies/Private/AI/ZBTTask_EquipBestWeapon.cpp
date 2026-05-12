// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_EquipBestWeapon.h"
#include "Characters/ZNPCharacter.h"
#include "Inventory/ZInventoryItemData.h"
#include "Components/ZEquipmentComponent.h"

UZBTTask_EquipBestWeapon::UZBTTask_EquipBestWeapon()
{
	NodeName = "Equip best weapon";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UZBTTask_EquipBestWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AZNPCharacter* Character = Cast<AZNPCharacter>(GetCharacter(OwnerComp));
	if (!Character)
	{
		return EBTNodeResult::Failed;
	}
	EquipmentComponent = Character->GetEquipmentComponent();
	if (EquipmentComponent->GetEquippedItem())
	{
		return EBTNodeResult::Failed;
	}
	const UZInventoryItemData* ItemData = Character->PickBestWeapon();
	if (!ItemData)
	{
		return EBTNodeResult::Failed;
	}

	if (!ItemData->ItemClass->IsChildOf(AZEquippableItem::StaticClass()))
	{
		EBTNodeResult::Failed;
	}

	TSubclassOf<AZEquippableItem> EquippableClass = ItemData->ItemClass.Get();
	EquipmentComponent->StartEquip(EquippableClass);
	if (!EquipmentComponent->GetEquippedItem() || !EquipmentComponent->GetEquippedItem()->IsEquipping())
	{
		return EBTNodeResult::Failed;
	}
	BTComp = &OwnerComp;
	OnEquipCompleteHandle = EquipmentComponent->OnEquipComplete.AddUObject(this, &ThisClass::OnFinished);
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UZBTTask_EquipBestWeapon::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EquipmentComponent->OnEquipComplete.Remove(OnEquipCompleteHandle);
	return EBTNodeResult::Aborted;
}

void UZBTTask_EquipBestWeapon::OnFinished(AZEquippableItem* Item)
{
	EquipmentComponent->OnEquipComplete.Remove(OnEquipCompleteHandle);
	FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
}

