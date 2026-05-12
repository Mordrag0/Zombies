// Copyright 2026 Luka Markuš. All rights reserved.


#include "Animation/ZZombieAnimInstance.h"
#include "Movement/ZZombieMovementComponent.h"
#include "Characters/ZZombieCharacter.h"

UZZombieAnimInstance::UZZombieAnimInstance()
{
	AttackType = EZZombieAttack::None;
}

void UZZombieAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ZombieCharacterOwner = Cast<AZZombieCharacter>(TryGetPawnOwner());
	if (ZombieCharacterOwner)
	{
		ZombieCharacterMovement = Cast<UZZombieMovementComponent>(ZombieCharacterOwner->GetMovementComponent());
	}
}

void UZZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!ZombieCharacterOwner || !ZombieCharacterMovement)
	{
		return;
	}

	const FVector Velocity = ZombieCharacterOwner->GetVelocity();
	const FVector LocalVelocity = ZombieCharacterOwner->GetActorRotation().UnrotateVector(Velocity);
	Speed = LocalVelocity.X / (ZombieCharacterMovement->MaxWalkSpeed);
}


