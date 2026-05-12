// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZAnimInstance.h"
#include "ZZombieAnimInstance.generated.h"

class AZZombieCharacter;
class UZZombieMovementComponent;
enum class EZZombieAttack : uint8;
/**
 * 
 */
UCLASS()
class ZOMBIES_API UZZombieAnimInstance : public UZAnimInstance
{
	GENERATED_BODY()
	
public:
	UZZombieAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EZZombieAttack AttackType;

	UPROPERTY()
	AZZombieCharacter* ZombieCharacterOwner;

	UPROPERTY()
	UZZombieMovementComponent* ZombieCharacterMovement;
};

