// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTTask_BlackboardBase.h"
#include "ZBTTask_WaitForPlayer.generated.h"

class AZFPCharacter;
class AZNPCharacter;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTTask_WaitForPlayer : public UZBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UZBTTask_WaitForPlayer();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	bool IsPlayerNearby(const AZFPCharacter* Player, const AZNPCharacter* CharacterOwner) const;

    UPROPERTY(EditAnywhere)
    float CheckInterval = 1.f;

    UPROPERTY(EditAnywhere)
    float WaitRadius = 300.f;
};

