// Copyright 2026 Luka Markuš. All rights reserved.


#include "Weapons/ZRangedWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Characters/ZCharacter.h"
#include "Inventory/ZInventoryComponent.h"
#include "Inventory/Items/ZAmmo.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "ZTypes.h"
#include "ZGameplayTags.h"

AZRangedWeapon::AZRangedWeapon()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);

	PrimaryActorTick.bCanEverTick = true;

	MuzzleSocketName = FName("MuzzleSocket");

	AmmoPerMag = 10;

	TimeOfLastShot = -1.f;
}

void AZRangedWeapon::PerformShot()
{
	Super::PerformShot();

	ConsumeAmmo();

	TimeOfLastShot = GetWorld()->GetTimeSeconds();
	if (CharacterOwner->IsLocallyControlled())
	{
		AddRecoil();
	}
}

bool AZRangedWeapon::CanFire() const
{
	if (!Super::CanFire())
	{
		return false;
	}
	return (GetAmmoInMag() > 0);
}

void AZRangedWeapon::ConsumeAmmo()
{
	if (HasAuthority())
	{
		AmmoInMag--;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, AmmoInMag, this);
	}
	else
	{
		PendingShots++;
	}
	OnAmmoChanged.Broadcast();
	if (AmmoInMag == 0)
	{
		OnOutOfAmmo.Broadcast();
		if (HasAuthority())
		{
			CharacterOwner->OnCharacterEvent.ExecuteIfBound(ZGameplayTags::AI_Weapon_OutOfAmmo);
		}
	}
}

void AZRangedWeapon::Reload()
{
	if (!ensure(HasAuthority()))
	{
		return;
	}
	if (!CharacterOwner->GetHasInfiniteAmmo())
	{
		const int32 AmmoInInventory = CharacterOwner->GetInventoryComponent()->GetItemCount(AmmoType);
		const int32 AmmoReloaded = FMath::Min(AmmoPerMag - AmmoInMag, AmmoInInventory);
		if (AmmoReloaded > 0)
		{
			CharacterOwner->GetInventoryComponent()->RemoveItem(AmmoType, AmmoReloaded, true);
		}
		AmmoInMag += AmmoReloaded;
	}
	else
	{
		AmmoInMag = AmmoPerMag;
	}
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, AmmoInMag, this);
	OnAmmoChanged.Broadcast();
	OnReload.Broadcast();

	if (IsReloading()) // When equipping we call Reload(), but we don't want to change the state to Ready
	{
		SetItemState(EZItemState::Ready);
	}
}

void AZRangedWeapon::OnEquip(AZCharacter* InOwner)
{
	Super::OnEquip(InOwner);
	
	if (CharacterOwner->IsLocallyControlled())
	{
		UZInventoryComponent* InventoryComponent = CharacterOwner->GetInventoryComponent();
		BIND_MULTICAST_UOBJECT(OnInventoryChangedConnection, InventoryComponent, InventoryComponent->OnInventoryChanged, this, &ThisClass::OnOwnerInventoryChanged);
	}

	if (HasAuthority())
	{
		Reload();
	}
}

void AZRangedWeapon::UnequipComplete()
{
	if (HasAuthority())
	{
		if (AmmoInMag > 0)
		{
			CharacterOwner->GetInventoryComponent()->AddItem(AmmoType, AmmoInMag, true);
		}
	}

	if (CharacterOwner->IsLocallyControlled())
	{
		OnInventoryChangedConnection.Disconnect();
	}
	Super::UnequipComplete();
}

void AZRangedWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, AmmoInMag, Params);
}

bool AZRangedWeapon::CanReload() const
{
	return (GetAmmoInMag() < AmmoPerMag) && OwnerHasAmmo() && (PendingShots == 0);
}

void AZRangedWeapon::StartReload(float Duration)
{
	if (!IsIdle() || !CanReload())
	{
		if (HasAuthority())
		{
			Client_Reliable_SetItemState(EZItemState::Ready);
		}
		return;
	}
	SetItemState(EZItemState::Reloading);
	if (CharacterOwner->HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Action, this, &ThisClass::Reload, Duration, false);
	}
	else
	{
		Server_Reliable_StartReload(Duration);
	}
}

bool AZRangedWeapon::OwnerHasAmmo() const
{
	return (CharacterOwner && (CharacterOwner->GetHasInfiniteAmmo() || (CharacterOwner->GetInventoryComponent()->GetItemCount(AmmoType) > 0)));
}

void AZRangedWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bRecoil)
	{
		const float WorldTime = GetWorld()->GetTimeSeconds();
		if (WorldTime > TimeOfLastShot)
		{
			const float TimePassed = WorldTime - TimeOfLastShot;
			if (TimePassed >= RecoilTime)
			{
				bRecoil = false;
				SetActorTickEnabled(false);
			}
			else
			{
				const float PreviousRecoilX = RecoilX->GetFloatValue(TimePassed - DeltaSeconds);
				const float NewRecoilX = RecoilX->GetFloatValue(TimePassed);
				const float DeltaRecoilX = NewRecoilX - PreviousRecoilX;
				CharacterOwner->AddControllerYawInput(bRecoilXDirection ? DeltaRecoilX : -DeltaRecoilX);
				const float PreviousRecoilY = RecoilY->GetFloatValue(TimePassed - DeltaSeconds);
				const float NewRecoilY = RecoilY->GetFloatValue(TimePassed);
				const float DeltaRecoilY = NewRecoilY - PreviousRecoilY;
				CharacterOwner->AddControllerPitchInput(-DeltaRecoilY);
			}
		}
	}
}

void AZRangedWeapon::PlayFireEffects()
{
	Super::PlayFireEffects();

	UMeshComponent* MeshComp = GetMesh();
	if (MeshComp)
	{
		if (MuzzleEffect)
		{
			MuzzleParticleComp = UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}
	}
}

void AZRangedWeapon::StopFireEffects()
{
	Super::StopFireEffects();
	
	if (MuzzleParticleComp)
	{
		MuzzleParticleComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		MuzzleParticleComp->Deactivate();
	}
}

void AZRangedWeapon::OnOwnerInventoryChanged()
{
	OnAmmoChanged.Broadcast(); // #ZTODO maybe on inventory changed should pass optional parameter about what changed, so we react only if it's null (multiple things changed) or this weapons ammo changed
}

void AZRangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);

	if (RecoilX && RecoilY)
	{
		float MinTime;
		float MaxTime;
		RecoilX->GetTimeRange(MinTime, MaxTime);
		RecoilTime = MaxTime;
		RecoilY->GetTimeRange(MinTime, MaxTime);
		RecoilTime = FMath::Max(RecoilTime, MaxTime);
	}

}

void AZRangedWeapon::AddRecoil()
{
	if (!RecoilX || !RecoilY)
	{
		return;
	}
	bRecoil = true;
	bRecoilXDirection = FMath::RandBool();
	SetActorTickEnabled(true);
}

void AZRangedWeapon::Server_Reliable_StartReload_Implementation(float Duration)
{
	StartReload(Duration);
}

void AZRangedWeapon::OnRep_AmmoInMag(int32 OldAmmo)
{
	if (AmmoInMag > OldAmmo)
	{
		PendingShots = 0;
	}
	else
	{
		int32 ServerShots = OldAmmo - AmmoInMag;
		PendingShots = FMath::Max(PendingShots - ServerShots, 0);
	}
	OnAmmoChanged.Broadcast();
}

