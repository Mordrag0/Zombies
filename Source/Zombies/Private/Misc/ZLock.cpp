// Copyright 2026 Luka Markuš. All rights reserved.


#include "Misc/ZLock.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "ZPlayerController.h"
#include "ZGameInstance.h"
#include "Characters/ZFPCharacter.h"
#include "Inventory/ZInventoryComponent.h"

AZLock::AZLock()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	OuterLockMesh = CreateDefaultSubobject<UStaticMeshComponent>("OuterLockMesh");
	OuterLockMesh->SetupAttachment(RootComponent);
	InnerLockMesh = CreateDefaultSubobject<UStaticMeshComponent>("InnerLockMesh");
	InnerLockMesh->SetupAttachment(RootComponent);
	LockpickMesh = CreateDefaultSubobject<UStaticMeshComponent>("LockpickMesh");
	LockpickMesh->SetupAttachment(RootComponent);
	ScrewdriverMesh = CreateDefaultSubobject<UStaticMeshComponent>("ScrewdriverMesh");
	ScrewdriverMesh->SetupAttachment(InnerLockMesh);

	LockpickingCamera = CreateDefaultSubobject<UCameraComponent>("Lockpicking camera");
	LockpickingCamera->SetupAttachment(RootComponent);

	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);

	MinLockpickRotation = -90.f;
	MaxLockpickRotation = 90.f;
	AcceptableDifference = 10.f;

	MinCylinderRotation = 0.f;
	MaxCylinderRotation = 90.f;

	BindingRange = 50.f;

	CylinderRotationSpeed = 50.f;

	TimeToBreakLockpick = .8f;
	BreakResetDuration = 1.5f;

	PrimaryActorTick.bCanEverTick = true;
}

void AZLock::StartLockpicking()
{
	SetActorHiddenInGame(false);

	CurrentLockpickAngle = 0.f;
	TargetLockpickAngle = FMath::FRandRange(MinLockpickRotation, MaxLockpickRotation);
	LockpickMesh->SetRelativeRotation(FRotator::ZeroRotator);

	CurrentCylinderRotation = 0.f;
	UpdateCylinderRotation();
	PrimaryActorTick.SetTickFunctionEnable(true);
}

void AZLock::StopLockpicking()
{
	SetActorHiddenInGame(true);
	PrimaryActorTick.SetTickFunctionEnable(false);
}

void AZLock::RotateLockpick(float X)
{
	if (bApplyingTension) // Allow rotating the lockpick only when not applying tension
	{
		return;
	}
	FRotator LockPickRotation = LockpickMesh->GetRelativeRotation();
	CurrentLockpickAngle = FMath::Clamp(LockPickRotation.Pitch + X, MinLockpickRotation, MaxLockpickRotation);
	LockPickRotation.Pitch = CurrentLockpickAngle;
	LockpickMesh->SetRelativeRotation(LockPickRotation);

	const float NewMaxCylinderRotationAmount = GetMaxCylinderRotationAmount();
	if (NewMaxCylinderRotationAmount < CurrentCylinderRotation)
	{
		CurrentCylinderRotation = NewMaxCylinderRotationAmount;
		UpdateCylinderRotation();
	}
}

void AZLock::TensionStart()
{
	bApplyingTension = true;
}

void AZLock::TensionEnd()
{
	bApplyingTension = false;
}

void AZLock::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bApplyingTension && !bPaused)
	{
		const float MaxCylinderRotationAmount = GetMaxCylinderRotationAmount();
		CurrentCylinderRotation = FMath::Min(CurrentCylinderRotation + CylinderRotationSpeed * DeltaSeconds, MaxCylinderRotationAmount);
		if (CurrentCylinderRotation >= MaxCylinderRotation)
		{
			OnUnlocked();
		}
		else if (CurrentCylinderRotation >= MaxCylinderRotationAmount)
		{
			TensionTimeAtMax += DeltaSeconds;
			if (TensionTimeAtMax >= TimeToBreakLockpick)
			{
				BreakLockpick();
			}
		}
	}
	else
	{
		CurrentCylinderRotation = FMath::Max(CurrentCylinderRotation - CylinderRotationSpeed * DeltaSeconds, MinCylinderRotation);
	}
	UpdateCylinderRotation();
}

void AZLock::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.SetTickFunctionEnable(false);
}

void AZLock::UpdateCylinderRotation()
{
	InnerLockMesh->SetRelativeRotation(FRotator(CurrentCylinderRotation, 0.f, 0.f));
}

float AZLock::GetMaxCylinderRotationAmount() const
{
	const float CylinderRange = MaxCylinderRotation - MinCylinderRotation;
	const float Difference = FMath::Abs(CurrentLockpickAngle - TargetLockpickAngle);

	if (FMath::IsNearlyEqual(BindingRange, AcceptableDifference))
    {
        return (Difference <= AcceptableDifference) ? CylinderRange : 0.f;
    }

	return CylinderRange * (1 - FMath::Clamp((Difference - AcceptableDifference) / (BindingRange - AcceptableDifference), 0.f, 1.f));
}

void AZLock::OnUnlocked()
{
	if (AZPlayerController* PC = Cast<AZPlayerController>(GetOwner()))
    {
        PC->OnLockpickingFinished(true);
    }
}

void AZLock::BreakLockpick()
{
	if (AZPlayerController* PC = Cast<AZPlayerController>(GetOwner()))
	{
		AZFPCharacter* Character = PC->GetFPCharacter();
		if (ensure(Character))
		{
			UZGameInstance* GI = GetWorld()->GetGameInstance<UZGameInstance>();
			if (ensure(GI))
			{
				UZInventoryComponent* InventoryComp = Character->GetInventoryComponent();
				const FPrimaryAssetId LockpickId = GI->GetLockpickID();
				const int32 LockpickCount = InventoryComp->GetItemCount(LockpickId);
				InventoryComp->RemoveItem(LockpickId, 1, true);
				if (LockpickCount > 1)
				{
					ResetLockpick();
				}
				else
				{
					PC->OnLockpickingFinished(false);
				}
			}
		}
	}
}

void AZLock::ResetLockpick()
{
	TensionTimeAtMax = 0.f;
	CurrentLockpickAngle = 0.f;
	bPaused = true;
	LockpickMesh->SetHiddenInGame(true);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Break, this, &ThisClass::Resume, BreakResetDuration, false);
}

void AZLock::Resume()
{
	bPaused = false;
	LockpickMesh->SetHiddenInGame(false);
}


