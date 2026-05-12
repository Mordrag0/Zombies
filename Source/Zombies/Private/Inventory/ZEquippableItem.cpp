// Copyright 2026 Luka Markuš. All rights reserved.


#include "Inventory/ZEquippableItem.h"
#include "ZLog.h"
#include "Characters/ZCharacter.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"
#include "Components/ZEquipmentComponent.h"
#include "ZTypes.h"

AZEquippableItem::AZEquippableItem()
{
	ItemState = EZItemState::Down;
	PendingState = EZItemState::None;

	ShowWeaponDelay = 1.f;
	EquipDuration = 2.f;
	HideWeaponDelay = 1.f;
	UnequipDuration = 2.f;
}

void AZEquippableItem::SetItemState(EZItemState InItemState)
{
	if (HasAuthority())
	{
		if (ItemState == InItemState)
		{
			UE_LOG(LogZItem, Warning, TEXT("Server same item state set: %s %s"), *CharacterOwner->GetName(), *ENUM_TO_STRING(EZItemState, InItemState));
			return;
		}
		ItemState = InItemState;
		if (!IsClientPredictedState(ItemState))
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ItemState, this);
		}
	}
	else if(CharacterOwner->IsLocallyControlled())
	{
		if (PredictedState == InItemState)
		{
			UE_LOG(LogZItem, Warning, TEXT("Client same item state set: %s %d"), *CharacterOwner->GetName(), (uint8)InItemState);
			return;
		}
		PredictedState = InItemState;
	}

	HandleItemStateChanged();
}

bool AZEquippableItem::IsDown() const
{
	return GetItemState() == EZItemState::Down;
}

bool AZEquippableItem::IsIdle() const
{
	return GetItemState() == EZItemState::Ready;
}

bool AZEquippableItem::IsEquipping() const
{
	return GetItemState() == EZItemState::Equipping;
}

bool AZEquippableItem::IsFiring() const
{
	return GetItemState() == EZItemState::Firing;
}

bool AZEquippableItem::IsHolding() const
{
	return GetItemState() == EZItemState::Holding;
}

bool AZEquippableItem::IsUnequipping() const
{
	return GetItemState() == EZItemState::Unequipping;
}

bool AZEquippableItem::IsReloading() const
{
	return GetItemState() == EZItemState::Reloading;
}

void AZEquippableItem::OnEquip(AZCharacter* InOwner)
{
	SetOwner(CharacterOwner);

	if (CharacterOwner->IsLocallyControlled())
	{
		if (UMeshComponent* Mesh = GetMesh())
		{
			Mesh->SetCastShadow(false);
		}
		
		AttachToComponent(CharacterOwner->GetActualMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GetAttachSocket());
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetItemState(EZItemState::Equipping);
	if (HasAuthority())
	{
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &ThisClass::SetVisible, true);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Visibility, Delegate, ShowWeaponDelay, false);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Action, this, &ThisClass::EquipComplete, EquipDuration, false);
	}
}

void AZEquippableItem::Unequip()
{
	if (!IsIdle())
	{
		if (CharacterOwner->IsLocallyControlled())
		{
			PendingState = EZItemState::Unequipping;
		}
		else
		{
			return;
		}
	}
	SetItemState(EZItemState::Unequipping);
	if (HasAuthority())
	{
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &ThisClass::SetVisible, false);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Visibility, Delegate, HideWeaponDelay, false);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Action, this, &ThisClass::UnequipComplete, UnequipDuration, false);
	}
	else
	{
		Server_Reliable_Unequip();
	}
}

void AZEquippableItem::EquipComplete()
{
	if (!ensure(HasAuthority()))
	{
		return;
	}
	if (!ensure(IsEquipping()))
	{
		return;
	}
	SetItemState(EZItemState::Ready);
	CharacterOwner->GetEquipmentComponent()->EquipComplete();
}

void AZEquippableItem::UnequipComplete()
{
	if (!ensure(HasAuthority()))
	{
		return;
	}
	if (!ensure(IsUnequipping()))
	{
		return;
	}
	SetItemState(EZItemState::Down);
	CharacterOwner->GetEquipmentComponent()->UnequipComplete();
}

void AZEquippableItem::FirePressed(EZItemInput Mode)
{
	UE_LOG(LogZItem, Warning, TEXT("Fire pressed %d (%s)"), (int32)Mode, *GetName());
}

void AZEquippableItem::FireReleased(EZItemInput Mode)
{
	UE_LOG(LogZItem, Warning, TEXT("Fire released %d (%s)"), (int32)Mode, *GetName());
}

void AZEquippableItem::BeginPlay()
{
	Super::BeginPlay();
}

bool AZEquippableItem::IsClientPredictedState(EZItemState InItemState) const
{
	return (InItemState == EZItemState::Firing) || (InItemState == EZItemState::Holding);
}

void AZEquippableItem::HandleItemStateChanged()
{
	OnItemStateChanged.Broadcast(ItemState);

	if (IsDown())
	{
		if (CharacterOwner && CharacterOwner->HasAuthority())
		{
			//SetOwner(nullptr);
		}
	}
	else if (IsIdle())
	{
		switch (PendingState)
		{
			case EZItemState::Down:
				break;
			case EZItemState::Ready:
				break;
			case EZItemState::Equipping:
				break;
			case EZItemState::Unequipping:
				Unequip();
				break;
			case EZItemState::Firing:
				break;
			case EZItemState::Reloading:
				break;
			case EZItemState::Holding:
				break;
			case EZItemState::None:
			default:
				break;
		}
		PendingState = EZItemState::None;
	}
}

void AZEquippableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ItemState, Params);
	
    FDoRepLifetimeParams Params2;
    Params2.bIsPushBased = true;
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bVisible, Params2);
}

void AZEquippableItem::SetVisible(bool bInVisible)
{
	bVisible = bInVisible;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bVisible, this);
	SetActorHiddenInGame(!bVisible);

	if (bVisible)
	{
		CharacterOwner->GetEquipmentComponent()->OnEquip.Broadcast(this);
	}
	else
	{
		CharacterOwner->GetEquipmentComponent()->OnUnequip.Broadcast(this);
	}
}

void AZEquippableItem::Client_Reliable_SetItemState_Implementation(EZItemState InItemState)
{
	ItemState = InItemState;
	PredictedState = InItemState;
}

void AZEquippableItem::Server_Reliable_Unequip_Implementation()
{
	Unequip();
}

void AZEquippableItem::OnRep_ItemState()
{
	PredictedState = ItemState; // #ZTODO what if you switch state twice quickly and it replicates the first one while pending state is set to the second one?
	HandleItemStateChanged();
}

void AZEquippableItem::OnRep_Visible()
{
	SetActorHiddenInGame(!bVisible);
	
	if (bVisible)
	{
		CharacterOwner->GetEquipmentComponent()->OnEquip.Broadcast(this);
	}
	else
	{
		CharacterOwner->GetEquipmentComponent()->OnUnequip.Broadcast(this);
	}
}

bool AZEquippableItem::CanInteract(const AZCharacter* Character) const
{
	return !CharacterOwner;
}

