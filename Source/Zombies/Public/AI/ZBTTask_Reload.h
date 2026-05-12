// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTTask_Base.h"
#include "ZBTTask_Reload.generated.h"

class AZRangedWeapon;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTTask_Reload : public UZBTTask_Base
{
	GENERATED_BODY()
	
public:
	UZBTTask_Reload();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	void OnFinished();

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BTComp;

	UPROPERTY()
	TObjectPtr<AZRangedWeapon> Weapon;

	FDelegateHandle OnReloadHandle;
};

