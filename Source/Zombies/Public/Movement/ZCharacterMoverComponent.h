// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DefaultMovementSet/CharacterMoverComponent.h"
#include "ZCharacterMoverComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogZCharacterMover, Log, All);

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZCharacterMoverComponent : public UCharacterMoverComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void Teleport(const FVector& Location);
	void Teleport(const FVector& Location, const FRotator& Rotation);
};

