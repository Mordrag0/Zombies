// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZBTTask_BlackboardBase.h"
#include "ZBTTask_NPCShoot.generated.h"

class UBlackboardComponent;
class AZCharacter;
class AZRangedWeapon;

struct FZNPCShootMemory
{
	TWeakObjectPtr<AZRangedWeapon> Weapon;
	FDelegateHandle OnWeaponStateChangedHandle;
};

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTTask_NPCShoot : public UZBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UZBTTask_NPCShoot();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	virtual uint16 GetInstanceMemorySize() const;

	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const;

protected:
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
};

