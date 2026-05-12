// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_Interact.h"
#include "ZInteractable.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/ZCharacter.h"

UZBTTask_Interact::UZBTTask_Interact()
{
	NodeName = "Interact";
	bCreateNodeInstance = true;

	INIT_TASK_NODE_NOTIFY_FLAGS();

	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, BlackboardKey), AActor::StaticClass());
	bAbort = false;
}

EBTNodeResult::Type UZBTTask_Interact::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BTComp = &OwnerComp;
	UBlackboardComponent* BBComp = GetBlackboard(OwnerComp);
	AActor* InteractableActor = Cast<AActor>(BBComp->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if (!InteractableActor || !InteractableActor->Implements<UZInteractable>())
	{
		return EBTNodeResult::Failed;
	}
	InteractableTarget = TScriptInterface<IZInteractable>(InteractableActor);
	AZCharacter* CharacterOwner = GetCharacter(OwnerComp);
	const bool bInteractionSuccess = CharacterOwner->StartInteractingWith(InteractableTarget);
	if (!bInteractionSuccess)
	{
		return EBTNodeResult::Failed;
	}
	OnInteractionStoppedHandle = CharacterOwner->OnInteractionStopped.AddUObject(this, &UZBTTask_Interact::OnInteractionStopped);
	return EBTNodeResult::InProgress;
}

void UZBTTask_Interact::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	AZCharacter* CharacterOwner = GetCharacter(OwnerComp);
	CharacterOwner->OnInteractionStopped.Remove(OnInteractionStoppedHandle);
}

EBTNodeResult::Type UZBTTask_Interact::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bAbort = true;
	AZCharacter* CharacterOwner = GetCharacter(OwnerComp);
	if (CharacterOwner && InteractableTarget)
	{
		CharacterOwner->StopInteractingWith(InteractableTarget); // Attempt stopping interaction
	}
	return EBTNodeResult::InProgress;
}

void UZBTTask_Interact::OnInteractionStopped(TScriptInterface<IZInteractable> Character, TScriptInterface<IZInteractable> Target)
{
	if (InteractableTarget == Target)
	{
		if (bAbort)
		{
			FinishLatentAbort(*BTComp);
		}
		else
		{
			FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
		}
	}
}

