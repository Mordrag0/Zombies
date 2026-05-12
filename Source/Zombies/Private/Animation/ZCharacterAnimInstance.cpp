// Copyright 2026 Luka Markuš. All rights reserved.


#include "Animation/ZCharacterAnimInstance.h"
#include "Characters/ZCharacter.h"
#include "Inventory/ZInventoryItem.h"
#include "Movement/ZCharacterMovementComponent.h"
#include "Inventory/ZEquippableItem.h"
#include "Components/ZEquipmentComponent.h"

void UZCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UZCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CharacterOwner)
	{
		return;
	}

	bADSing = CharacterOwner->GetADSing();
	UZEquipmentComponent* EquipmentComponent = CharacterOwner->GetEquipmentComponent();

	EquippedItemType = EquipmentComponent->GetEquippedItemType();
	ItemState = EquipmentComponent->GetEquippedItemState();

	//CrouchAmount = CharacterMovement->GetCrouchAmount(); // #ZTODOMOVER
}

