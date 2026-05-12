// Copyright 2026 Luka Markuš. All rights reserved.


#include "Components/ZStaminaComponent.h"
#include "Characters/ZCharacter.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

UZStaminaComponent::UZStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 0.1f;
	
	MaxStamina = 100.f;
	SprintDrainRate = 15.f;
	WalkRegenRate = 30;
	IdleRegenRate = 40.f;
	ExhaustedTime = 1.f;

	bServerAllowsSprint = true;
	bExhausted = false;

	SetIsReplicatedByDefault(true);
}

void UZStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterOwner = GetOwner<AZCharacter>();

	if (GetOwner()->GetLocalRole() > ROLE_SimulatedProxy)
	{
		SetStamina(MaxStamina);
		SetComponentTickEnabled(true);
	}
}

void UZStaminaComponent::SetStamina(float InStamina)
{
	if (GetOwner()->HasAuthority())
	{
		if (Stamina == InStamina)
		{
			return;
		}
		Stamina = InStamina;
		OnStaminaChanged.Broadcast(Stamina, MaxStamina);
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Stamina, this);
	}
	else
	{
		if (PredictedStamina == InStamina)
		{
			return;
		}
		PredictedStamina = InStamina;
		OnStaminaChanged.Broadcast(PredictedStamina, MaxStamina);
	}
}

void UZStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CharacterOwner)
	{
		return;
	}

	float NewStamina = GetOwner()->HasAuthority() ? Stamina : PredictedStamina;

	// if (CharacterMovement->GetGaitState() == EZGaitState::Sprint) // #ZTODOMOVER
	// {
	// 	NewStamina -= SprintDrainRate * DeltaTime;
	// }
	// else if (CharacterMovement->IsMoving())
	// {
	// 	NewStamina += WalkRegenRate * DeltaTime;
	// }
	// else
	// {
	// 	NewStamina += IdleRegenRate * DeltaTime;
	// }

	NewStamina = FMath::Clamp(NewStamina, 0.f, MaxStamina);
	SetStamina(NewStamina);
	if (NewStamina <= 0.f && !bExhausted)
	{
		OnExhaustedStart();
	}
}

void UZStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
	Params.Condition = COND_OwnerOnly;

    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Stamina, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bServerAllowsSprint, Params);
}

void UZStaminaComponent::OnExhaustedStart()
{
	bExhausted = true;
	if (GetOwner()->HasAuthority())
	{
		bServerAllowsSprint = false;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bServerAllowsSprint, this);
	}
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Exhausted, this, &ThisClass::OnExhaustedEnd, ExhaustedTime);
	OnCanSprintChanged.ExecuteIfBound();
}

void UZStaminaComponent::OnExhaustedEnd()
{
	bExhausted = false;
	if (GetOwner()->HasAuthority())
	{
		bServerAllowsSprint = true;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bServerAllowsSprint, this);
	}
	OnCanSprintChanged.ExecuteIfBound();
}

void UZStaminaComponent::OnRep_Stamina()
{
	if (Stamina < PredictedStamina)
	{
		PredictedStamina = Stamina;
		OnStaminaChanged.Broadcast(PredictedStamina, MaxStamina);
	}
}

void UZStaminaComponent::OnRep_ServerAllowsSprint()
{
	OnCanSprintChanged.ExecuteIfBound();
}


