// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZCharacterMovementComponent.h"
#include "ZZombieMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZZombieMovementComponent : public UZCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetSpeed(float InSpeed);
	
	//BEGIN UMovementComponent Interface
	virtual float GetMaxSpeed() const override;
	//END UMovementComponent Interface
protected:

	UPROPERTY(BlueprintReadOnly)
	float Speed = 500.f;
};

