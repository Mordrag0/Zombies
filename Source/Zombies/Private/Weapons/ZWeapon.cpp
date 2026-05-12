// Copyright 2026 Luka Markuš. All rights reserved.


#include "Weapons/ZWeapon.h"
#include "Weapons/ZWeaponAction.h"
#include "Weapons/ZImpactEffect.h"
#include "Components/MeshComponent.h"
#include "Characters/ZFPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "ZTypes.h"


void AZWeapon::FirePressed(EZItemInput Mode)
{
	if (Actions.Contains(Mode))
	{
		if (!Actions[Mode]->IsHeldDown())
		{
			Actions[Mode]->FirePressed();
		}
	}
}

void AZWeapon::FireReleased(EZItemInput Mode)
{
	if (Actions.Contains(Mode))
	{
		if (Actions[Mode]->IsHeldDown())
		{
			Actions[Mode]->FireReleased();
		}
	}
}

void AZWeapon::OnEquip(AZCharacter* InOwner)
{
	Super::OnEquip(InOwner);

	if (InOwner->IsLocallyControlled())
	{
		InitWeaponActions();
	}
}

void AZWeapon::UnequipComplete()
{
	Super::UnequipComplete();

	DestroyWeaponActions();
	StopFireEffects();
}

void AZWeapon::PerformShot()
{
	if (!HasAuthority())
	{
		Server_Reliable_PerformShot();
	}
	SetItemState(EZItemState::Firing);
	PlayFireEffects();
}

void AZWeapon::FireComplete()
{
	if (!HasAuthority())
	{
		Server_Reliable_FireComplete();
	}
	SetItemState(EZItemState::Ready);
}

bool AZWeapon::CanFire() const
{
	if (IsIdle())
	{
		return true;
	}
	if (IsEquipping())
	{
		return false;
	}
	if (IsFiring())
	{
		return true;
	}
	return false;
}

void AZWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AZWeapon::InitWeaponActions()
{
	if (bWeaponActionsInitialized)
	{
		return;
	}
	bWeaponActionsInitialized = true;

	int32 Idx = 0;
	for (const TTuple<EZItemInput, TSubclassOf<UZWeaponAction>>& KVP : ActionClassList)
	{
		if (KVP.Value)
		{
			const EZItemInput ActionType = KVP.Key;
			if (ensure(!Actions.Contains(ActionType)))
			{
				UZWeaponAction* NewAction = NewObject<UZWeaponAction>(this, KVP.Value);
				Actions.Add(ActionType, NewAction);
				NewAction->Initialize(this);
			}
		}
		Idx++;
	}
}

void AZWeapon::SpawnImpactEffect(const FHitResult& Impact)
{
	if (ImpactEffect && Impact.bBlockingHit)
	{
		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint);
		AZImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<AZImpactEffect>(ImpactEffect, SpawnTransform);
		if (EffectActor)
		{
			EffectActor->SetSurfaceHit(Impact);
			EffectActor->FinishSpawning(SpawnTransform);
		}
	}
}

void AZWeapon::PlayFireEffects()
{
	if (HasAuthority())
	{
		Multicast_Unreliable_PlayFireEffects();
	}
	if (FireSound)
	{
		if (Cast<AZFPCharacter>(CharacterOwner))
		{
			FireSoundComp = UGameplayStatics::SpawnSound2D(this, FireSound, 1.f, 1.f, 0.f, ConcurrencySettings);
		}
		else
		{
			FVector SocketLoc;
			FRotator SocketRot;
			const UMeshComponent* MeshComp = GetMesh();
			MeshComp->GetSocketWorldLocationAndRotation(MuzzleSocketName, SocketLoc, SocketRot);
			FireSoundComp = UGameplayStatics::SpawnSoundAtLocation(
				this,
				FireSound,
				SocketLoc,
				SocketRot,
				1.f,
				1.f,
				0.f,
				AttenuationSettings3P,
				ConcurrencySettings3P
			);
		}
	}
}

void AZWeapon::StopFireEffects()
{
	if (FireSoundComp)
	{
		FireSoundComp->Stop();
		FireSoundComp = nullptr;
	}
}

void AZWeapon::Multicast_Unreliable_PlayFireEffects_Implementation()
{
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		PlayFireEffects();
	}
}

void AZWeapon::Server_Reliable_PerformShot_Implementation()
{
	PerformShot();
}

void AZWeapon::Server_Reliable_FireComplete_Implementation()
{
	FireComplete();
}

void AZWeapon::DestroyWeaponActions()
{
	bWeaponActionsInitialized = false;
	Actions.Empty(ActionClassList.Num());
}


