// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZAnimInstance.h"
#include "ZCharacterAnimInstance.generated.h"

class AZCharacter;
class UZCharacterMovementComponent;
enum class EZEquipType : uint8;
enum class EZItemState : uint8;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZCharacterAnimInstance : public UZAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EZEquipType EquippedItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EZItemState ItemState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bADSing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CrouchAmount;
};

