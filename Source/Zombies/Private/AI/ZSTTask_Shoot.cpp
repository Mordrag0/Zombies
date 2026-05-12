// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_Shoot.h"
#include "StateTreeExecutionContext.h"
#include "ZTypes.h"
#include "Weapons/ZRangedWeapon.h"
#include "Components/ZEquipmentComponent.h"
#include "Characters/ZNPCharacter.h"
#include "AI/ZNPCAIController.h"
#include "StateTreeAsyncExecutionContext.h"
#include "Characters/ZCharacter.h"


EStateTreeRunStatus FZSTTask_Shoot::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character)
	{
		return EStateTreeRunStatus::Failed;
	}
	InstanceData.Weapon = Cast<AZRangedWeapon>(InstanceData.Character->GetEquipmentComponent()->GetEquippedItem());
	if (!InstanceData.Weapon || !InstanceData.Weapon->OwnerHasAmmo())
	{
		return EStateTreeRunStatus::Failed;
	}
	if (!InstanceData.Target || InstanceData.Target->GetDead())
	{
		return EStateTreeRunStatus::Failed;
	}
	BIND_MULTICAST(InstanceData.OnOutOfAmmoConnection, InstanceData.Weapon, InstanceData.Weapon->OnOutOfAmmo, 
		[WeakContext = Context.MakeWeakExecutionContext()]()
	{
		WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
	});
	BIND_MULTICAST(InstanceData.OnTargetDied, InstanceData.Target, InstanceData.Target->OnDeath,
		[WeakContext = Context.MakeWeakExecutionContext()]
	{
		WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
	});
	InstanceData.Character->SetFocusActor(InstanceData.Target);
	InstanceData.Weapon->FirePressed(EZItemInput::Primary);
	return EStateTreeRunStatus::Running;
}

void FZSTTask_Shoot::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.OnOutOfAmmoConnection.Disconnect();
	InstanceData.OnTargetDied.Disconnect();
	if (InstanceData.Character->GetFocusActor() == InstanceData.Target)
	{
		InstanceData.Character->ClearFocusActor();
	}
	if (InstanceData.Weapon)
	{
		InstanceData.Weapon->FireReleased(EZItemInput::Primary);
	}
}

#if WITH_EDITOR
FText FZSTTask_Shoot::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return GET_OBJECT_DISPLAY_TEXT(Target);
}
#endif

