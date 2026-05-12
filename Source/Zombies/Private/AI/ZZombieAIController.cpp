// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZZombieAIController.h"
#include "Perception/AIPerceptionSystem.h"
#include "Characters/ZZombieCharacter.h"
#include "BrainComponent.h"


void AZZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ZombieCharacter = Cast<AZZombieCharacter>(InPawn);
}

