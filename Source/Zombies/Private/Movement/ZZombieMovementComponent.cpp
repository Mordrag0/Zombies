// Copyright 2026 Luka Markuš. All rights reserved.


#include "Movement/ZZombieMovementComponent.h"

void UZZombieMovementComponent::SetSpeed(float InSpeed)
{
	Speed = InSpeed;
}

float UZZombieMovementComponent::GetMaxSpeed() const
{
	return Speed;
}

