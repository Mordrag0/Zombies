// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZStaminaWidget.h"
#include "Characters/ZFPCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/ZStaminaComponent.h"

void UZStaminaWidget::Init(AZFPCharacter* Character)
{
	StaminaComponent = Character->GetStaminaComponent();

	BIND_MULTICAST_UOBJECT(OnStaminaChangedConnection, StaminaComponent, StaminaComponent->OnStaminaChanged, this, &ThisClass::OnStaminaChanged);
	OnStaminaChanged(StaminaComponent->GetStamina(), StaminaComponent->GetMaxStamina());
}

void UZStaminaWidget::NativeDestruct()
{
	OnStaminaChangedConnection.Disconnect();
	Super::NativeDestruct();
}

void UZStaminaWidget::OnStaminaChanged(float Stamina, float MaxStamina)
{
	StaminaBar->SetPercent(Stamina / MaxStamina);
}

