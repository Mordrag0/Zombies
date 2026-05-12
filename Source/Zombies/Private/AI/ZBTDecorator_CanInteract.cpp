// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTDecorator_CanInteract.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/ZCharacter.h"
#include "ZInteractable.h"

UZBTDecorator_CanInteract::UZBTDecorator_CanInteract()
{
	NodeName = "Can interact";
}

bool UZBTDecorator_CanInteract::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AZCharacter* Character = GetCharacter(OwnerComp);
	UBlackboardComponent* BBComp = GetBlackboard(OwnerComp);
	AActor* InteractableActor = Cast<AActor>(BBComp->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if (!InteractableActor || !InteractableActor->Implements<UZInteractable>())
	{
		return false;
	}
	IZInteractable* Interactable = Cast<IZInteractable>(InteractableActor);
	return Interactable->CanInteract(Character);
}

