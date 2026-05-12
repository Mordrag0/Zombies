// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZAIController.h"
#include "ZZombieAIController.generated.h"

class AZZombieCharacter;

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZZombieAIController : public AZAIController
{
	GENERATED_BODY()
	
public:

protected:

	virtual void OnPossess(APawn* InPawn) override;

	TObjectPtr<AZZombieCharacter> ZombieCharacter;
};

