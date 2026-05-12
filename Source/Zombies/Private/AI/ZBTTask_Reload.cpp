// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_Reload.h"
#include "Characters/ZCharacter.h"
#include "Weapons/ZRangedWeapon.h"
#include "Components/ZEquipmentComponent.h"
#include "ZTypes.h"

UZBTTask_Reload::UZBTTask_Reload()
{
	NodeName = "Reload";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UZBTTask_Reload::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AZCharacter* Character = GetCharacter(OwnerComp);
	if (!Character)
	{
		return EBTNodeResult::Failed;
	}
	Weapon = Cast<AZRangedWeapon>(Character->GetEquipmentComponent()->GetEquippedItem());
	if (!Weapon)
	{
		return EBTNodeResult::Failed;
	}
	if (!Weapon->CanReload())
	{
		return EBTNodeResult::Failed;
	}
	Weapon->FirePressed(EZItemInput::Reload);
	Weapon->FireReleased(EZItemInput::Reload);
	if (!Weapon->IsReloading())
	{
		return EBTNodeResult::Failed;
	}
	BTComp = &OwnerComp;
	OnReloadHandle = Weapon->OnReload.AddUObject(this, &ThisClass::OnFinished);
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UZBTTask_Reload::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Weapon->OnReload.Remove(OnReloadHandle);
	return EBTNodeResult::Aborted;
}

void UZBTTask_Reload::OnFinished()
{
	Weapon->OnReload.Remove(OnReloadHandle);
	FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
}

