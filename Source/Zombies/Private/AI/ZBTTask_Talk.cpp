// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_Talk.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/ZFPCharacter.h"
#include "Characters/ZNPCharacter.h"
#include "ZPlayerController.h"
#include "ZTypes.h"

UZBTTask_Talk::UZBTTask_Talk()
{
	NodeName = "Talk";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UZBTTask_Talk::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bAborted = false;
	BTComp = &OwnerComp;
	UBlackboardComponent* BlackboardComp = GetBlackboard(OwnerComp);
	AZFPCharacter* Target = Cast<AZFPCharacter>(BlackboardComp->GetValueAsObject(GetSelectedBlackboardKey()));
	if (!Target)
	{
		return EBTNodeResult::Failed;
	}
	CharacterOwner = Cast<AZNPCharacter>(GetCharacter(OwnerComp));
	if (!CharacterOwner)
	{
		return EBTNodeResult::Failed;
	}
	const bool bInteractionSuccess = CharacterOwner->StartInteractingWith(Target);
	if (!bInteractionSuccess)
	{
		return EBTNodeResult::Failed;
	}
	OnDialogueEndedHandle = CharacterOwner->OnInteractionStopped.AddUObject(this, &ThisClass::OnDialogueEnded);
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UZBTTask_Talk::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bAborted = true; // #ZTODO: Allow aborting this task
	return EBTNodeResult::InProgress;
}

void UZBTTask_Talk::OnDialogueEnded(TScriptInterface<IZInteractable> NPC, TScriptInterface<IZInteractable> Player)
{
	ensure(NPC == CharacterOwner);
	if (CharacterOwner)
	{
		CharacterOwner->OnInteractionStopped.Remove(OnDialogueEndedHandle);
		OnDialogueEndedHandle.Reset();
	}
	if (bAborted)
	{
		FinishLatentAbort(*BTComp);
	}
	else
	{
		FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
	}
}

