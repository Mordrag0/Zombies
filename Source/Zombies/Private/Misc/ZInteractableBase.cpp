// Copyright 2026 Luka Markuš. All rights reserved.


#include "Misc/ZInteractableBase.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Characters/ZCharacter.h"
#include "ZPlayerController.h"
#include "ZTypes.h"

AZInteractableBase::AZInteractableBase()
{
	bReplicates = true;
}

void AZInteractableBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InteractingCharacter, Params);
}

bool AZInteractableBase::SetInteractionTarget(TScriptInterface<IZInteractable> InInteractionTarget)
{
	ensure(HasAuthority()); // Setting the interaction target should always be called on the server, the RPCs go through AZFPCharacter
	if (!ensure(!InteractionTarget && InInteractionTarget))
	{
		return false;
	}
	InteractionTarget = InInteractionTarget;
	InteractingCharacter = Cast<AZCharacter>(InteractionTarget.GetObject());
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, InteractingCharacter, this);

	ensure(InteractingCharacter);
	OnInteractionStarted.Broadcast(this, InteractionTarget);
	return true;
}

void AZInteractableBase::ClearInteractionTarget(TScriptInterface<IZInteractable> InInteractionTarget)
{
	ensure(HasAuthority()); // Clearing the interaction target should always be called on the server, the RPCs go through AZFPCharacter
	if (!ensure(InteractionTarget && (InteractionTarget == InInteractionTarget)))
	{
		return;
	}
	OnInteractionStopped.Broadcast(this, InteractionTarget);
	InteractionTarget = nullptr;
	InteractingCharacter = nullptr;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, InteractingCharacter, this);
}

bool AZInteractableBase::CanInteract(const AZCharacter* InCharacter) const
{
	return !InteractionTarget && InCharacter->IsActivityCompatible(GetCharacterActivity());
}

bool AZInteractableBase::BeginInteract(AZCharacter* InCharacter)
{
	if (!CanInteract(InCharacter))
	{
		ensure(0);
		return false;
	}
	if (!SetInteractionTarget(InCharacter))
	{
		ensure(0); // CanInteract() should have returned false in this case
		return false;
	}
	return true;
}

void AZInteractableBase::EndInteract(AZCharacter* InCharacter)
{
	if (!ensure(InteractingCharacter == InCharacter))
	{
		return;
	}
	ClearInteractionTarget(InCharacter);
}

EZCharacterActivity AZInteractableBase::GetCharacterActivity() const
{
	unimplemented(); return EZCharacterActivity::None;
}

void AZInteractableBase::OnRep_InteractingCharacter()
{
	InteractionTarget = TScriptInterface<IZInteractable>(InteractingCharacter);
}


