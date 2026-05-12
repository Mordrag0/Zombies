// Copyright 2026 Luka Markuš. All rights reserved.


#include "Components/ZEquipmentComponent.h"
#include "Characters/ZCharacter.h"
#include "EnhancedInputComponent.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/ZEquippableItem.h"
#include "ZTypes.h"
#include "ZGameplayTags.h"
#include "ZLog.h"

UZEquipmentComponent::UZEquipmentComponent()
{
	SetIsReplicatedByDefault(true);
}

void UZEquipmentComponent::StartEquip(TSubclassOf<AZEquippableItem> ItemClass)
{
	if (EquippedItem)
	{
		PendingEquipItemClass = ItemClass;
		StartUnequip();
		return;
	}

	if (!GetOwner()->HasAuthority())
	{
		Server_Reliable_StartEquip(ItemClass);
		return;
	}

	FTransform Transform;
	AZEquippableItem* Item = GetWorld()->SpawnActorDeferred<AZEquippableItem>(ItemClass, FTransform::Identity, CharacterOwner, CharacterOwner, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Item->FinishSpawning(FTransform::Identity);

	Item->AttachToComponent(CharacterOwner->GetActualMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Item->GetAttachSocket());
	Item->OnEquip(CharacterOwner);
	EquippedItem = Item;

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, EquippedItem, this);
	OnEquip.Broadcast(Item);
}

void UZEquipmentComponent::StartUnequip()
{
	if (!EquippedItem)
	{
		UE_LOG(LogZombies, Verbose, TEXT("%s trying to unequip but nothing is equipped."), *GetName());
		return;
	}
	EquippedItem->Unequip();
	OnUnequip.Broadcast(EquippedItem);
}

void UZEquipmentComponent::EquipComplete()
{
	OnEquipComplete.Broadcast(EquippedItem);
	CharacterOwner->OnCharacterEvent.ExecuteIfBound(ZGameplayTags::AI_Item_EquipComplete);
}

void UZEquipmentComponent::UnequipComplete()
{
	ensure(CharacterOwner->HasAuthority());

	EquippedItem->Destroy();
	EquippedItem = nullptr;

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, EquippedItem, this);
	OnUnequipComplete.Broadcast(EquippedItem);

	if (PendingEquipItemClass)
	{
		StartEquip(PendingEquipItemClass);
	}
}

EZEquipType UZEquipmentComponent::GetEquippedItemType() const
{
	if (EquippedItem)
	{
		return EquippedItem->GetEquipType();
	}
	else
	{
		return EZEquipType::None;
	}
}

EZItemState UZEquipmentComponent::GetEquippedItemState() const
{
	if (EquippedItem)
	{
		return EquippedItem->GetItemState();
	}
	else
	{
		return EZItemState::Down;
	}
}

void UZEquipmentComponent::SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
	ensure(EnhancedInputComponent);

	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &UZEquipmentComponent::FirePressed<EZItemInput::Primary>);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &UZEquipmentComponent::FireReleased<EZItemInput::Primary>);

	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &UZEquipmentComponent::FirePressed<EZItemInput::Reload>);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Completed, this, &UZEquipmentComponent::FireReleased<EZItemInput::Reload>);

	EnhancedInputComponent->BindAction(ADSAction, ETriggerEvent::Started, this, &UZEquipmentComponent::FirePressed<EZItemInput::Secondary>);
	EnhancedInputComponent->BindAction(ADSAction, ETriggerEvent::Completed, this, &UZEquipmentComponent::FireReleased<EZItemInput::Secondary>);
}

void UZEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;

    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, EquippedItem, Params);
}

void UZEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterOwner = Cast<AZCharacter>(GetOwner());
}

void UZEquipmentComponent::Server_Reliable_StartEquip_Implementation(TSubclassOf<AZEquippableItem> ItemClass)
{
	StartEquip(ItemClass);
}

void UZEquipmentComponent::OnRep_EquippedItem(AZEquippableItem* OldItem)
{
	if (OldItem)
	{
		OnUnequipComplete.Broadcast(OldItem);
	}
	if (EquippedItem)
	{
		EquippedItem->OnEquip(CharacterOwner);
	}
}


