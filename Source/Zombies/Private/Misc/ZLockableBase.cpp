// Copyright 2026 Luka Markuš. All rights reserved.


#include "Misc/ZLockableBase.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "ZGameInstance.h"
#include "ZPlayerController.h"
#include "Characters/ZCharacter.h"
#include "Inventory/ZInventoryComponent.h"
#include "ZTypes.h"

AZLockableBase::AZLockableBase()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	LockTransformComponent = CreateDefaultSubobject<USceneComponent>("Lock transform component");
	LockTransformComponent->SetupAttachment(RootComponent);

	PivotComponent = CreateDefaultSubobject<USceneComponent>("Pivot component");
	PivotComponent->SetupAttachment(RootComponent);
}

void AZLockableBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bLocked, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bOpen, Params);
}

void AZLockableBase::Lock()
{
	bLocked = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bLocked, this);
}

void AZLockableBase::Unlock()
{
	bLocked = false;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bLocked, this);
}

void AZLockableBase::Open()
{
	if (bOpen)
	{
		return;
	}
	bOpen = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bOpen, this);
}

void AZLockableBase::Close()
{
	if (!bOpen)
	{
		return;
	}
	bOpen = false;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bOpen, this);
}

bool AZLockableBase::CanInteract(const AZCharacter* Character) const
{
	if (!Super::CanInteract(Character))
	{
		return false;
	}
	if (bLocked)
	{
		UZGameInstance* GI = GetWorld()->GetGameInstance<UZGameInstance>();
		if (!GI || Character->GetInventoryComponent()->GetItemCount(GI->GetLockpickID()) <= 0)
		{
			return false; // #ZTODO: notify the player he's out of lockpicks
		}
		if (!Character->HasSkill(EZSkill::Lockpicking))
		{
			return false;
		}
	}
	return true;
}

FTransform AZLockableBase::GetPivotPoint() const
{
	return PivotComponent->GetComponentTransform();
}

EZCharacterActivity AZLockableBase::GetCharacterActivity() const
{
	return EZCharacterActivity::Lockpicking;
}


