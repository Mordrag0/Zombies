// Copyright 2026 Luka Markuš. All rights reserved.


#include "Misc/ZContainer.h"
#include "Characters/ZCharacter.h"
#include "ZGameMode.h"
#include "ZPlayerController.h"
#include "Inventory/ZInventoryComponent.h"
#include "Components/SceneComponent.h"
#include "ZTypes.h"

AZContainer::AZContainer()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	InventoryComponent = CreateDefaultSubobject<UZInventoryComponent>("Inventory");
}

bool AZContainer::BeginInteract(AZCharacter* InCharacter)
{
	if (GetLocked()) // Base class handles locked state
	{
		return Super::BeginInteract(InCharacter);
	}
	
	if (!CanInteract(InCharacter))
	{
		return false;
	}
	if (!SetInteractionTarget(InCharacter))
	{
		ensure(0); // Should be prevented by CanInteract check
		return false;
	}
	Open();
	return true;
}

void AZContainer::EndInteract(AZCharacter* InCharacter)
{
	Super::EndInteract(InCharacter);
	Close();
}

EZCharacterActivity AZContainer::GetCharacterActivity() const
{
	return GetLocked() ? EZCharacterActivity::Lockpicking : EZCharacterActivity::Looting;
}

FZInteractionParams AZContainer::GetInteractionParams(const AZCharacter* InCharacter) const
{
	if (GetCharacterActivity() == EZCharacterActivity::Looting)
	{
		return FZInteractionParams(TInPlaceType<FZLootingParams>{}, InventoryComponent);
	}
	return FZInteractionParams(TInPlaceType<FZNoInteractionParams>{});
}

void AZContainer::Unlock()
{
	if (ensure(InteractingCharacter))
	{
		const bool bActivityEnded = InteractingCharacter->TriggerEndingActivity(GetCharacterActivity());
		Super::Unlock();
		// #ZTODO if !bActivityEnded then we need to wait
		InteractingCharacter->StartInteractionActivity(FZInteractionState(this, GetCharacterActivity(), GetInteractionParams(InteractingCharacter)));
		Open();
	}
}



