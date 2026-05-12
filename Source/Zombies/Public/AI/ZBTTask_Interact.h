// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTTask_BlackboardBase.h"
#include "ZBTTask_Interact.generated.h"

class UBehaviorTreeComponent;
class IZInteractable;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTTask_Interact : public UZBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UZBTTask_Interact();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	void OnInteractionStopped(TScriptInterface<IZInteractable> Character, TScriptInterface<IZInteractable> Target);

	FDelegateHandle OnInteractionStoppedHandle;

	UPROPERTY()
	TScriptInterface<IZInteractable> InteractableTarget;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BTComp;

	bool bAbort;
};

