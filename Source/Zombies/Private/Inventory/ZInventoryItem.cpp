// Copyright 2026 Luka Markuš. All rights reserved.


#include "Inventory/ZInventoryItem.h"
#include "Characters/ZCharacter.h"
#include "Inventory/ZInventoryComponent.h"
#include "Inventory/ZInventoryItemData.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "ZTypes.h"

DEFINE_LOG_CATEGORY(LogZItem)

AZInventoryItem::AZInventoryItem()
{
	bReplicates = true;

	Count = 1;
}

bool AZInventoryItem::SetInteractionTarget(TScriptInterface<IZInteractable> InInteractionTarget)
{
	if (!ensure(!InteractionTarget && InInteractionTarget))
	{
		return false;
	}
	InteractionTarget = InInteractionTarget;
	return true;
}

bool AZInventoryItem::CanInteract(const AZCharacter* InCharacter) const
{
	return !bPickedUp && InCharacter->IsActivityCompatible(GetCharacterActivity());
}

bool AZInventoryItem::BeginInteract(AZCharacter* InCharacter)
{
	if (!CanInteract(InCharacter))
	{
		ensure(0);
		return false;
	}
	if (!SetInteractionTarget(InCharacter))
	{
		return false;
	}
	InCharacter->StartPickUp(this);

	return true;
}

void AZInventoryItem::EndInteract(AZCharacter* InCharacter)
{
	if (!ensure(InteractionTarget == InCharacter))
	{
		return;
	}
}

TArray<UMeshComponent*> AZInventoryItem::GetHighlightMeshes() const
{
	return { GetMesh() };
}

FTransform AZInventoryItem::GetPivotPoint() const
{
	return GetActorTransform();
}

EZCharacterActivity AZInventoryItem::GetCharacterActivity() const
{
	return EZCharacterActivity::PickingUp;
}

FPrimaryAssetId AZInventoryItem::GetItemId() const
{
	return ItemData ? ItemData->GetPrimaryAssetId() : FPrimaryAssetId();
}

void AZInventoryItem::StartPickUp(AZCharacter* Character)
{
	bPickedUp = true;
	OnInteractionStarted.Broadcast(this, Character);
	OnInteractionStopped.Broadcast(this, Character);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bPickedUp, this);
}

void AZInventoryItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bPickedUp, Params);
}

void AZInventoryItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

	CharacterOwner = Cast<AZCharacter>(NewOwner);
}

void AZInventoryItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AZInventoryItem::OnRep_Owner()
{
	Super::OnRep_Owner();

	CharacterOwner = Cast<AZCharacter>(GetOwner());
}


