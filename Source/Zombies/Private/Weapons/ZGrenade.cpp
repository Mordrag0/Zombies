// Copyright 2026 Luka Markuš. All rights reserved.


#include "Weapons/ZGrenade.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ZConstants.h"
#include "Characters/ZCharacter.h"
#include "Inventory/ZInventoryComponent.h"
#include "Misc/ZExplosionEffect.h"
#include "Inventory/ZInventoryItemData.h"
#include "Components/ZEquipmentComponent.h"
#include "ZTypes.h"

AZGrenade::AZGrenade()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	RootComponent = Mesh;

	ExplosionPoint = CreateDefaultSubobject<USceneComponent>("ExplosionPoint");
	ExplosionPoint->SetupAttachment(Mesh);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovementComponent->bAutoActivate = false;

	EquipType = EZEquipType::Grenade;

	FuseTime = 3.f;
	DamageInnerRadius = 200.f;
	DamageOuterRadius = 100.f;
	Damage = 90.f;
	MinimumDamage = 25.f;
	DamageFalloff = 1.f;

	ReleaseTime = 1.f;

	SetReplicateMovement(true);
}

void AZGrenade::FirePressed(EZItemInput Mode)
{
	if (!IsIdle())
	{
		// UE_LOG(LogZEquippableItem, Warning, TEXT("Not Idle"));
		return;
	}
	StartHolding();
}

void AZGrenade::FireReleased(EZItemInput Mode)
{
	if (!IsHolding())
	{
		// UE_LOG(LogZEquippableItem, Warning, TEXT("Not Holding"));
		return;
	}
	StartThrowing();
}

void AZGrenade::Release()
{
	if (!ensure(HasAuthority()))
	{
		return;
	}
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CharacterOwner->GetInventoryComponent()->RemoveItem(ItemData->GetPrimaryAssetId(), 1, true);
	CharacterOwner->GetEquipmentComponent()->ClearEquippedItem();
	GetMesh()->SetCollisionProfileName(FName("Projectile"));
	SetActorEnableCollision(true);
	SetActorRotation(CharacterOwner->GetControlRotation());
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector(ProjectileMovementComponent->InitialSpeed, 0.f, 0.f));
	ProjectileMovementComponent->Activate();
	bReleased = true;
}

void AZGrenade::StartHolding()
{
	SetItemState(EZItemState::Holding);
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fuse, this, &ThisClass::Explode, FuseTime, false);
	}
}

void AZGrenade::Server_Reliable_StartHolding_Implementation()
{
	StartHolding();
}

void AZGrenade::StartThrowing()
{
	SetItemState(EZItemState::Firing);
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Action, this, &ThisClass::Release, ReleaseTime, false);
	}
}

void AZGrenade::Server_Reliable_StartThrowing_Implementation()
{
	StartThrowing();
}

void AZGrenade::Multicast_Unreliable_PlayExplosionEffects_Implementation()
{
	if (ExplosionEffect)
	{
		GetWorld()->SpawnActor(ExplosionEffect, &ExplosionPoint->GetComponentTransform());
	}
}

void AZGrenade::DestroyGrenade()
{
	Destroy();
}

void AZGrenade::Explode()
{
	if (!bReleased)
	{
		Release();
	}
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	const FVector Loc = ExplosionPoint->GetComponentLocation();
	UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, MinimumDamage, Loc, DamageInnerRadius, DamageOuterRadius, DamageFalloff, DamageTypeClass, IgnoreActors, this, GetInstigatorController(), ECC_WeaponTrace);

	Multicast_Unreliable_PlayExplosionEffects();

	SetActorHiddenInGame(true);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Destroy, this, &ThisClass::DestroyGrenade, 2.f);
}

