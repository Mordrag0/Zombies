// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_Reload.h"
#include "Characters/ZNPCharacter.h"
#include "Components/ZEquipmentComponent.h"
#include "Weapons/ZRangedWeapon.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeAsyncExecutionContext.h"
#include "ZTypes.h"

EStateTreeRunStatus FZSTTask_Reload::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character)
	{
		return EStateTreeRunStatus::Failed;
	}
	UZEquipmentComponent* EquipmentComponent = InstanceData.Character->GetEquipmentComponent();
	AZRangedWeapon* RangedWeapon = Cast<AZRangedWeapon>(EquipmentComponent->GetEquippedItem());
	if (!RangedWeapon)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (RangedWeapon->IsIdle())
	{
		FStateTreeWeakExecutionContext WeakContext = Context.MakeWeakExecutionContext();
		Reload(WeakContext);
	}
	else
	{
		BIND_MULTICAST(InstanceData.OnWaitForIdleConnection, RangedWeapon, RangedWeapon->OnItemStateChanged,
			[WeakContext = Context.MakeWeakExecutionContext(), this](EZItemState ItemState)
		{
			FStateTreeStrongExecutionContext StrongContext = WeakContext.MakeStrongExecutionContext();
			if (StrongContext.IsValid())
			{
				if (ItemState == EZItemState::Ready)
				{
					if (FInstanceDataType* InstanceData = StrongContext.GetInstanceDataPtr<FInstanceDataType>())
					{
						InstanceData->OnWaitForIdleConnection.Disconnect();
						Reload(WeakContext);
					}
				}
			}
		});
	}
	return EStateTreeRunStatus::Running;
}

void FZSTTask_Reload::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.OnWaitForIdleConnection.Disconnect();
	InstanceData.OnReloadCompleteConnection.Disconnect();
}

void FZSTTask_Reload::Reload(FStateTreeWeakExecutionContext WeakContext) const
{
	FStateTreeStrongExecutionContext StrongContext = WeakContext.MakeStrongExecutionContext();
	if (StrongContext.IsValid())
	{
		if (FInstanceDataType* InstanceData = StrongContext.GetInstanceDataPtr<FInstanceDataType>())
		{
			if (!InstanceData->Character)
			{
				WeakContext.FinishTask(EStateTreeFinishTaskType::Failed);
				return;
			}
			UZEquipmentComponent* EquipmentComponent = InstanceData->Character->GetEquipmentComponent();
			AZRangedWeapon* RangedWeapon = Cast<AZRangedWeapon>(EquipmentComponent->GetEquippedItem());
			if (!RangedWeapon || !RangedWeapon->OwnerHasAmmo())
			{
				WeakContext.FinishTask(EStateTreeFinishTaskType::Failed);
				return;
			}
			BIND_MULTICAST(InstanceData->OnReloadCompleteConnection, RangedWeapon, RangedWeapon->OnReload,
				[WeakContext]()
			{
				WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
			});
			RangedWeapon->FirePressed(EZItemInput::Reload);
			RangedWeapon->FireReleased(EZItemInput::Reload);
		}
	}
}

