// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTTask_BlackboardBase.h"
#include "ZBTTask_Talk.generated.h"

class AZNPCharacter;
class UBehaviorTreeComponent;
class AZCharacter;
class IZInteractable;
enum class EZCharacterActivity : uint32;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTTask_Talk : public UZBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UZBTTask_Talk();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void OnDialogueEnded(TScriptInterface<IZInteractable> NPC, TScriptInterface<IZInteractable> Player);

	bool bAborted;

	FDelegateHandle OnDialogueEndedHandle;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BTComp;

	UPROPERTY()
	TObjectPtr<AZNPCharacter> CharacterOwner; // #ZTODO do we need to store this? Can we get it from the BTComp every time?
};

