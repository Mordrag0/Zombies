// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTTask_Base.h"
#include "ZBTTask_Unequip.generated.h"

class AZEquippableItem;
class UZEquipmentComponent;
class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTTask_Unequip : public UZBTTask_Base
{
	GENERATED_BODY()
	
public:
	UZBTTask_Unequip();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	void OnFinished(AZEquippableItem* Item);

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BTComp;

	UPROPERTY()
	TObjectPtr<UZEquipmentComponent> EquipmentComponent;

	FDelegateHandle OnUnequipHandle;
};

