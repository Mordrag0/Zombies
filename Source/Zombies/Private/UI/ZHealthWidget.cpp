// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZHealthWidget.h"
#include "Components/ProgressBar.h"
#include "Characters/ZFPCharacter.h"

void UZHealthWidget::Init(AZFPCharacter* Character)
{
	BIND_MULTICAST_UOBJECT(OnHealthChangedConnection, Character, Character->OnHealthChanged, this, &ThisClass::OnHealthChanged);
	OnHealthChanged(Character->GetHealth(), Character->GetMaxHealth());
}

void UZHealthWidget::NativeDestruct()
{
	OnHealthChangedConnection.Disconnect();
	Super::NativeDestruct();
}

void UZHealthWidget::OnHealthChanged(float Health, float MaxHealth)
{
	HealthBar->SetPercent(Health / MaxHealth);
}
