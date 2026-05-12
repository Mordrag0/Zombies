// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZAIController.h"
#include "Perception/AIPerceptionSystem.h"
#include "Characters/ZCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "ZGameState.h"
#include "Components/StateTreeAIComponent.h"
#include "ZGameplayTags.h"
#include "StructUtils/InstancedStruct.h"
#include "AI/ZStateTreePayloads.h"

AZAIController::AZAIController()
{
	StateTreeComponent = CreateDefaultSubobject<UStateTreeAIComponent>("StateTreeComponent");
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComponent");

	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("SightConfig");
	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 2200.f;
	SightConfig->PeripheralVisionAngleDegrees = 70.f;
	SightConfig->SetMaxAge(10.f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 0.f; // #ZTODO if this value > 0 then we automatically keep seeing the target if it didn't move more than this value since last time it was seen, but what this means is that the owner can move far away and will still automatically see the target as long as it doesn't move (might need to override UAISense_Sight::ShouldAutomaticallySeeTarget)
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AZAIController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (NewTeamID == GetGenericTeamId())
	{
		return;
	}
	Super::SetGenericTeamId(NewTeamID);

	UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(GetWorld());
	PerceptionSystem->UpdateListener(*GetAIPerceptionComponent());
}

ETeamAttitude::Type AZAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* OtherPawn = Cast<const APawn>(&Other);
	if (!OtherPawn)
	{
		return ETeamAttitude::Type::Neutral;
	}

	const IGenericTeamAgentInterface* OtherControllerTeamAgent = Cast<const IGenericTeamAgentInterface>(OtherPawn->GetController());
	if (!OtherControllerTeamAgent)
	{
		return ETeamAttitude::Neutral;
	}
	FGenericTeamId OtherTeamId = OtherControllerTeamAgent->GetGenericTeamId();

	AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
	if (!GS)
	{
		return ETeamAttitude::Neutral;
	}
	return GS->GetAttitudeTowards(static_cast<EZFaction>(GetGenericTeamId().GetId()), static_cast<EZFaction>(OtherTeamId.GetId()));
}

void AZAIController::BeginPlay()
{
	Super::BeginPlay();
	
	PerceptionComponent->OnTargetPerceptionUpdated.RemoveAll(this); // #ZTODO
	PerceptionComponent->OnTargetPerceptionForgotten.RemoveAll(this); // #ZTODO
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
	PerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ThisClass::OnTargetPerceptionForgotten);
}

bool AZAIController::ShouldAttack(AActor* Other) const
{
	return GetTeamAttitudeTowards(*Other) == ETeamAttitude::Hostile;
}

bool AZAIController::ShouldLookAt(AActor* Other) const
{
	if (APawn* OtherPawn = Cast<APawn>(Other))
	{
		return OtherPawn->IsPlayerControlled();
	}
	return false;
}

void AZAIController::SetCombatTarget(AZCharacter* CharacterTarget)
{
	if (CombatTarget == CharacterTarget)
	{
		return;
	}
	CombatTarget = CharacterTarget;
	if (CombatTarget)
	{
		BIND_MULTICAST_UOBJECT(OnCombatTargetDeathConnection, CombatTarget, CombatTarget->OnDeath, this, &ThisClass::OnCombatTargetDied, CombatTarget.Get());
		SendStateTreeEvent(ZGameplayTags::AI_Combat_TargetSet);
	}
	else
	{
		OnCombatTargetDeathConnection.Disconnect();
		SendStateTreeEvent(ZGameplayTags::AI_Combat_TargetCleared);
	}
}

AZCharacter* AZAIController::GetCombatTarget() const
{
	return CombatTarget;
}

void AZAIController::OnCombatTargetDied(AZCharacter* InCharacter)
{
	if (CombatTarget != InCharacter) // Make sure the perception system didn't already update the combat target
	{
		return;
	}
	SetCombatTarget(nullptr);
}

void AZAIController::SetLookAtTarget(AActor* Actor)
{
	ZCharacter->SetLookAtTarget(Actor);
}

void AZAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ZCharacter = Cast<AZCharacter>(InPawn);
	BIND_MULTICAST_UOBJECT(OnDeathConnection, ZCharacter, ZCharacter->OnDeath, this, &ThisClass::OnDeath);
	BIND_DELEGATE_UOBJECT(OnCharacterEventConnection, ZCharacter, ZCharacter->OnCharacterEvent, this, &ThisClass::OnCharacterEvent);

	const EZFaction Faction = ZCharacter->GetFaction();
	SetGenericTeamId(static_cast<uint8>(Faction));
}

void AZAIController::OnUnPossess()
{
	StateTreeComponent->StopLogic(TEXT("Unpossessed"));
	OnCombatTargetDeathConnection.Disconnect();
	OnDeathConnection.Disconnect();
	OnCharacterEventConnection.Disconnect();
	ZCharacter = nullptr;

	Super::OnUnPossess();
}

void AZAIController::OnDeath()
{
	StateTreeComponent->StopLogic(TEXT("Death"));
	SetCombatTarget(nullptr);

	UnPossess();
}

void AZAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!GetPawn())
	{
		return;
	}
	if (!Stimulus.WasSuccessfullySensed())
	{
		return;
	}
	TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);
	if (SenseClass != UAISense_Sight::StaticClass())
	{
		return; // #ZTODO add hearing logic
	}
	if (AZCharacter* TargetCharacter = Cast<AZCharacter>(Actor))
	{
		if (ShouldAttack(TargetCharacter))
		{
			SensedTargetActors.Add(TargetCharacter);
			if (!GetCombatTarget())
			{
				SetCombatTarget(TargetCharacter);
			}
		}
	}
	if (ShouldLookAt(Actor))
	{
		SensedLookAtActors.Add(Actor);
		OnSensedLookAtActorAdded(Actor);
		if (!ZCharacter->GetLookAtTarget())
		{
			SetLookAtTarget(Actor);
		}
	}
}

void AZAIController::OnTargetPerceptionForgotten(AActor* Actor)
{
	if (!GetPawn())
	{
		return;
	}
	if (AZCharacter* TargetCharacter = Cast<AZCharacter>(Actor))
	{
		SensedTargetActors.Remove(TargetCharacter);
		if (GetCombatTarget() == TargetCharacter)
		{
			// Replace target with closest sensed actor
			if (SensedTargetActors.Num() == 0)
			{
				SetCombatTarget(nullptr);
			}
			else
			{
				const FVector PawnLocation = GetPawn()->GetActorLocation();
				float ClosestDistSquared = MAX_FLT;
				AZCharacter* ClosestSensedActor = nullptr;
				for (AZCharacter* SensedActor : SensedTargetActors)
				{
					if (ShouldAttack(SensedActor))
					{
						const float DistSquared = FVector::DistSquared(PawnLocation, SensedActor->GetActorLocation());
						if (DistSquared < ClosestDistSquared)
						{
							ClosestDistSquared = DistSquared;
							ClosestSensedActor = SensedActor;
						}
					}
				}
				SetCombatTarget(ClosestSensedActor);
			}
		}
	}

	SensedLookAtActors.Remove(Actor);
	OnSensedLookAtActorRemoved(Actor);
	if (ZCharacter && (ZCharacter->GetLookAtTarget() == Actor))
	{
		if (SensedLookAtActors.Num() == 0)
		{
			ZCharacter->ClearLookAtTarget();
		}
		else
		{
			const FVector PawnLocation = GetPawn()->GetActorLocation();
			float ClosestDistSquared = MAX_FLT;
			AActor* ClosestSensedActor = nullptr;
			for (AActor* SensedActor : SensedLookAtActors)
			{
				const float DistSquared = FVector::DistSquared(PawnLocation, SensedActor->GetActorLocation());
				if (DistSquared < ClosestDistSquared)
				{
					ClosestDistSquared = DistSquared;
					ClosestSensedActor = SensedActor;
				}
			}
			SetLookAtTarget(ClosestSensedActor);
		}
	}
}

void AZAIController::OnCharacterEvent(FGameplayTag EventTag)
{
	SendStateTreeEvent(EventTag);
}

void AZAIController::SendStateTreeEvent(FGameplayTag EventTag)
{
	PreSendStateTreeEvent(EventTag);
	StateTreeComponent->SendStateTreeEvent(FStateTreeEvent(EventTag));
}

void AZAIController::SendStateTreeEvent(FGameplayTag EventTag, const FInstancedStruct& Payload)
{
	PreSendStateTreeEvent(EventTag, Payload);
	StateTreeComponent->SendStateTreeEvent(EventTag, Payload);
}

