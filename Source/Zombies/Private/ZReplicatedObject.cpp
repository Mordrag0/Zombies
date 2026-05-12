// Copyright 2026 Luka Markuš. All rights reserved.


#include "ZReplicatedObject.h"
#include "Characters/ZCharacter.h"
#include "Weapons/ZWeapon.h"

UWorld* UZReplicatedObject::GetWorld() const
{
	if (const UObject* MyOuter = GetOuter())
	{
		return MyOuter->GetWorld();
	}
	return nullptr;
}

int32 UZReplicatedObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	check(GetOuter());
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}

bool UZReplicatedObject::CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));

	AActor* Owner = GetOwningActor();
	if (UNetDriver* NetDriver = Owner->GetNetDriver())
	{
		NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
		return true;
	}
	return false;
}

void UZReplicatedObject::Destroy()
{
	if (IsValid(this))
	{
		checkf(GetOwningActor()->HasAuthority(), TEXT("Destroy:: Object does not have authority to destroy itself!"));

		OnDestroyed();
	}
}

bool UZReplicatedObject::IsLocallyOwned() const
{
	const AZCharacter* CharacterOwner = Cast<AZCharacter>(GetOwningActor());
	if (!CharacterOwner)
	{
		if (const AZWeapon* WeaponOwner = Cast<AZWeapon>(GetOwningActor())) // #ZTODO: why not inventory item
		{
			CharacterOwner = WeaponOwner->GetCharacterOwner();
		}
	}
	return (IsOwnedByLocalRemote() || (CharacterOwner && CharacterOwner->IsLocallyControlled()));
}

bool UZReplicatedObject::IsNonOwningAuthority() const
{
	return (IsAuthority() && !IsLocallyOwned());
}

