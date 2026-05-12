// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZNPCAIController.h"
#include "Characters/ZNPCharacter.h"
#include "ZGameState.h"
#include "Events/ZEvent.h"
#include "AI/ZPath.h"
#include "Characters/ZFPCharacter.h"
#include "ZPlayerController.h"
#include "AI/ZWaypoint.h"
#include "Inventory/ZInventoryItem.h"
#include "Misc/ZLockableBase.h"
#include "Inventory/ZInventoryComponent.h"
#include "ZGameplayStatics.h"
#include "ZGameplayTags.h"
#include "AI/ZStateTreePayloads.h"

FZPathData::FZPathData(FGameplayTag InPathTag, AZNPCharacter* InNPCharacter, AZFPCharacter* InPlayer, bool bInEscortPlayer)
{
	PathTag = InPathTag;
	NPCharacter = InNPCharacter;
	AZGameState* GS = NPCharacter->GetWorld()->GetGameState<AZGameState>();
	Path = GS->GetPath(InPathTag);
	WaypointIndex = 0;
	Player = InPlayer;
	bEscortPlayer = bInEscortPlayer;
}

bool FZPathData::IncrementWaypointIndex()
{
	WaypointIndex++;
	if (IsComplete())
	{
		AZGameState* GS = NPCharacter->GetWorld()->GetGameState<AZGameState>();
		GS->CompletePath(Path, NPCharacter, Player ? Player->GetController<AZPlayerController>() : nullptr);
		return true;
	}
	return false;
}

bool FZPathData::IsComplete() const
{
	return Path && (Path->GetWaypoints().Num() == WaypointIndex);
}

AZNPCAIController::AZNPCAIController()
{
	HomeLocation = FAISystem::InvalidLocation;
	HomeRotation = FAISystem::InvalidRotation;

	CachedDesiredActivity = EZCharacterActivity::None;
	CachedStatePriority = EZNPCStatePriority::MAX;
}

void AZNPCAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	RegisterPriorityStates();
}

void AZNPCAIController::OnDialogueEventCompleted(const FZDialogueOptionRow* DialogueOption, AZPlayerController* EventInstigator)
{
	if (NPCharacter->IsPendingDialogue(EventInstigator, DialogueOption->ID))
	{
		NPCharacter->RemovePendingDialogue(EventInstigator, DialogueOption->ID);
		UpdatePendingDialogueTarget();
	}
	if (DialogueOption->DialogueContext != EZDialogueContext::None)
	{
		NPCharacter->CompleteContextDialogue(DialogueOption->ID);
	}
}

void AZNPCAIController::OnDialogueEventAvailable(const FZDialogueOptionRow* DialogueOption, AZPlayerController* EventInstigator)
{
	if (DialogueOption->IsNPCInitiated())
	{
		NPCharacter->AddPendingDialogue(EventInstigator, DialogueOption->ID, DialogueOption->DialogueContext);
		UpdatePendingDialogueTarget();
	}
}

void AZNPCAIController::OnDialogueEventUnavailable(const FZDialogueOptionRow* DialogueOption, AZPlayerController* EventInstigator)
{
	if (NPCharacter->IsPendingDialogue(EventInstigator, DialogueOption->ID))
	{
		NPCharacter->RemovePendingDialogue(EventInstigator, DialogueOption->ID);
		UpdatePendingDialogueTarget();
	}
}

EZCharacterActivity AZNPCAIController::GetDesiredActivity(int32 Hour) const
{
	if (UZGameplayStatics::IsHourBetween(Hour, NPCharacter->GetSleepStartTime(), NPCharacter->GetSleepEndTime()))
	{
		return NPCharacter->GetAssignedInteractable(EZCharacterActivity::Sleeping) ? EZCharacterActivity::Sleeping : EZCharacterActivity::None;
	}
	return NPCharacter->GetAssignedInteractable(EZCharacterActivity::Sitting) ? EZCharacterActivity::Sitting : EZCharacterActivity::None;
}

void AZNPCAIController::Attack(AZPlayerController* PC)
{
	AZFPCharacter* Player = PC->GetFPCharacter();
	SetCombatTarget(Player);
	StolenItems.Remove(PC);
	DetectedBreakIns.Remove(PC);
	ClearSpottedCriminal(Player);
}

void AZNPCAIController::TakeBackStolenItems(AZPlayerController* PC)
{
	AZFPCharacter* Player = PC->GetFPCharacter();
	if (ensure(StolenItems.Contains(PC)))
	{
		const TMap<FPrimaryAssetId, int32>& PlayerStolenItems = StolenItems[PC].Items;
		TArray<FZItemStack> Items;
		for (const TPair<FPrimaryAssetId, int32>& KVP : PlayerStolenItems)
		{
			Items.Add(FZItemStack(KVP));
		}
		FZTradeRequest TradeRequest;
		TradeRequest.PlayerItems = Items;
		TradeRequest.NPCInventory = NPCharacter->GetInventoryComponent();
		if (!Player->GetInventoryComponent()->AcceptTrade(TradeRequest, true))
		{
			// #ZTODO: notify that items were missing
			SetCombatTarget(Player);
		}
		StolenItems.Remove(PC);
	}
	ClearSpottedCriminal(Player);
}

void AZNPCAIController::WarnAboutBreakingIn(AZPlayerController* PC)
{
	DetectedBreakIns.Remove(PC);
	ClearSpottedCriminal(PC->GetFPCharacter()); // #ZTODO handle repeated warnings, there should be consequences
}

void AZNPCAIController::StartPath(FGameplayTag PathTag, AZPlayerController* EventInstigator, bool bEscortPlayer)
{
	if (const AZGameState* GS = GetWorld()->GetGameState<AZGameState>())
	{
		if (ensure(!PathData.IsValid()))
		{
			PathData = FZPathData(PathTag, NPCharacter, EventInstigator->GetFPCharacter(), bEscortPlayer);
			SendStateTreeEvent(ZGameplayTags::AI_Path_Started);
		}
	}
}

void AZNPCAIController::CancelPath(FGameplayTag PathTag)
{
	if (const AZGameState* GS = GetWorld()->GetGameState<AZGameState>())
	{
		if (PathTag == PathData.PathTag)
		{
			PathData.Reset();
			SendStateTreeEvent(ZGameplayTags::AI_Path_Ended);
		}
	}
}

void AZNPCAIController::CompletePath(FGameplayTag PathTag)
{
	if (const AZGameState* GS = GetWorld()->GetGameState<AZGameState>())
	{
		if (PathTag == PathData.PathTag)
		{
			PathData.Reset();
			SendStateTreeEvent(ZGameplayTags::AI_Path_Ended);
		}
	}
}

bool AZNPCAIController::CanEnterPriorityState(EZNPCStatePriority Priority) const
{
	return (Priority == CachedStatePriority); // #ZTODO prevent entering TalkTo if we are in SpottedCriminal state or PendingDialogue state
}

bool AZNPCAIController::IsHighestPriorityState(EZNPCStatePriority Priority) const
{
	return (Priority == CachedStatePriority);
}

void AZNPCAIController::HandleHourChanged(int32 Hour)
{
	const EZCharacterActivity OldDesiredActivity = CachedDesiredActivity;
	CachedDesiredActivity = GetDesiredActivity(Hour);
	if (OldDesiredActivity != CachedDesiredActivity)
	{
		SendStateTreeEvent(ZGameplayTags::AI_Activity_DesiredActivityChanged);
	}
}

void AZNPCAIController::SetHomeTransform(FGameplayTag WaypointTag)
{
	if (const AZGameState* GS = GetWorld()->GetGameState<AZGameState>())
	{
		if (const AZWaypoint* Waypoint = GS->GetWaypoint(WaypointTag))
		{
			HomeLocation = Waypoint->GetActorLocation();
			HomeRotation = Waypoint->GetActorRotation();
			SendStateTreeEvent(ZGameplayTags::AI_HomeTransformChanged);
		}
	}
}

void AZNPCAIController::RegisterPriorityStates()
{
	const FZNPCPriorityStateEntry PriorityStates[static_cast<uint8>(EZNPCStatePriority::MAX)] = {
		{
			EZNPCStatePriority::Combat,
			[this]() { return !!CombatTarget; },
			FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{
				ZGameplayTags::AI_Combat_TargetSet,
				ZGameplayTags::AI_Combat_TargetCleared
			})
		},
		{
			EZNPCStatePriority::ConfrontCriminal,
			[this]() { return !!SpottedCriminal; },
			FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{
				ZGameplayTags::AI_Dialogue_SpottedCriminalSet,
				ZGameplayTags::AI_Dialogue_SpottedCriminalCleared
			})
		},
		{
			EZNPCStatePriority::TalkToPlayer,
			[this]() { return !!PendingDialogueTarget; },
			FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{
				ZGameplayTags::AI_Dialogue_PendingTargetSet,
				ZGameplayTags::AI_Dialogue_PendingTargetCleared
			})
		},
		{
			EZNPCStatePriority::FollowPath,
			[this]() { return PathData.IsValid(); },
			FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{
				ZGameplayTags::AI_Path_Started,
				ZGameplayTags::AI_Path_Ended
			})
		},
		{
			EZNPCStatePriority::Activity,
			[this]() { return CachedDesiredActivity != EZCharacterActivity::None; },
			FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{
				ZGameplayTags::AI_Activity_DesiredActivityChanged
			})
		},
	};

	for (const FZNPCPriorityStateEntry& Entry : PriorityStates)
	{
		PriorityConditions[static_cast<uint8>(Entry.Priority)] = Entry.Condition;
		PriorityStateEvents.AppendTags(Entry.RelevantEvents);
	}
}

void AZNPCAIController::OnPossess(APawn* InPawn)
{
	NPCharacter = Cast<AZNPCharacter>(InPawn);
	Super::OnPossess(InPawn);
	
	if (!ensureMsgf(NPCharacter, TEXT("%s: pawn %s is not a AZNPCharacter"), *GetName(), *InPawn->GetName()))
	{
		return;
	}

	BIND_MULTICAST_UOBJECT(OnInteractionStartedConnection, NPCharacter, NPCharacter->OnInteractionStarted, this, &ThisClass::OnInteractionStarted);
	BIND_MULTICAST_UOBJECT(OnInteractionStoppedConnection, NPCharacter, NPCharacter->OnInteractionStopped, this, &ThisClass::OnInteractionStopped);

	HomeLocation = InPawn->GetActorLocation(); // #ZTODO get from a NPC manager
	HomeRotation = InPawn->GetActorRotation();

	if (AZGameState* GS = GetWorld()->GetGameState<AZGameState>())
	{
		GS->RegisterActor(GS->GetNPCAIControllers(), this, NPCharacter->GetFullNPCName());
		if (GS->GetInitialized())
		{
			CachedDesiredActivity = GetDesiredActivity(GS->GetHour());
			CachedStatePriority = GetCurrentHighestPriorityState(); // Must be called after CachedDesiredActivity is set, if GS is not valid then Hour Changed event will be triggered as it initializes
		}
	}

	for (AActor* InteractableActor : NPCharacter->GetOwnedItems())
	{
		IZInteractable* Interactable = Cast<IZInteractable>(InteractableActor);
		check(Interactable);
		BIND_MULTICAST_UOBJECT(OnOwnedItemsInteractionStartedConnections.Add(InteractableActor), Interactable, Interactable->OnInteractionStarted, this, &ThisClass::OnOwnedItemInteractionStarted);
	}
}

void AZNPCAIController::OnUnPossess()
{
	Super::OnUnPossess();

	if (AZGameState* GS = GetWorld()->GetGameState<AZGameState>())
	{
		GS->UnregisterActor(GS->GetNPCAIControllers(), NPCharacter->GetFullNPCName());
	}
	OnInteractionStartedConnection.Disconnect();
	OnInteractionStoppedConnection.Disconnect();
	OnOwnedItemsInteractionStartedConnections.Empty();

	NPCharacter = nullptr;
}

void AZNPCAIController::OnInteractionStarted(TScriptInterface<IZInteractable> NPC, TScriptInterface<IZInteractable> Player)
{
	ensure(NPC.GetObject() == NPCharacter);
	if (Cast<AZCharacter>(Player.GetObject()))
	{
		InteractingCharacter = Cast<AZCharacter>(Player.GetObject());
		SendStateTreeEvent(ZGameplayTags::AI_Dialogue_TargetSet);
	}
}

void AZNPCAIController::OnInteractionStopped(TScriptInterface<IZInteractable> NPC, TScriptInterface<IZInteractable> Player)
{
	ensure(NPC.GetObject() == NPCharacter);
	if (InteractingCharacter == Player.GetObject())
	{
		InteractingCharacter = nullptr;
		SendStateTreeEvent(ZGameplayTags::AI_Dialogue_TargetCleared);
	}
}

void AZNPCAIController::OnOwnedItemInteractionStarted(TScriptInterface<IZInteractable> OwnedItem, TScriptInterface<IZInteractable> InInteractingCharacter)
{
	OnOwnedItemsInteractionStartedConnections.Remove(Cast<AActor>(OwnedItem.GetObject()));
	if (SpottedCriminal) // #ZTODO maybe support multiple spotted criminals?
	{
		return;
	}
	AZFPCharacter* Player = Cast<AZFPCharacter>(InInteractingCharacter.GetObject());
	if (!Player)
	{
		return;
	}
	AZPlayerController* PC = Player->GetController<AZPlayerController>();
	if (!SensedLookAtActors.Contains(Player))
	{
		return; // Didn't see the act
	}
	if (AZInventoryItem* InventoryItem = Cast<AZInventoryItem>(OwnedItem.GetObject()))
	{
		FZStolenItems& PlayerStolenItems = StolenItems.FindOrAdd(PC);
		int32& Count = PlayerStolenItems.Items.FindOrAdd(InventoryItem->GetItemId());
		Count += InventoryItem->GetCount();
		SpottedCriminal = Player;
		NPCharacter->Confront(PC, EZDialogueContext::Confrontation_Stealing);
		SendStateTreeEvent(ZGameplayTags::AI_Dialogue_SpottedCriminalSet);
	}
	else if (AZLockableBase* LockableActor = Cast<AZLockableBase>(OwnedItem.GetObject()))
	{
		DetectedBreakIns.Add(PC, LockableActor);
		SpottedCriminal = Player;
		NPCharacter->Confront(PC, EZDialogueContext::Confrontation_BreakingIn);
		SendStateTreeEvent(ZGameplayTags::AI_Dialogue_SpottedCriminalSet);
	}
}

void AZNPCAIController::OnSensedLookAtActorAdded(AActor* SensedActor)
{
	AZFPCharacter* Player = Cast<AZFPCharacter>(SensedActor);
	if (!Player)
	{
		return;
	}
	UpdatePendingDialogueTarget();
	if (SpottedCriminal)
	{
		return;
	}
	AZPlayerController* PC = Player->GetController<AZPlayerController>();
	TScriptInterface<IZInteractable> InteractingTarget = Player->GetInteractionTarget(EZCharacterActivity::Lockpicking);
	const bool bCaughtInAct = InteractingTarget && NPCharacter->GetOwnedItems().Contains(InteractingTarget.GetObject());
	const bool bKnownThief = StolenItems.Contains(PC);
	const bool bKnownBurglar = DetectedBreakIns.Contains(PC);
	if (bCaughtInAct || bKnownThief || bKnownBurglar)
	{
		SpottedCriminal = Player;
		NPCharacter->Confront(PC, bKnownThief ? EZDialogueContext::Confrontation_Stealing : EZDialogueContext::Confrontation_BreakingIn);
		SendStateTreeEvent(ZGameplayTags::AI_Dialogue_SpottedCriminalSet);
	}
}

void AZNPCAIController::OnSensedLookAtActorRemoved(AActor* SensedActor)
{
	AZFPCharacter* Player = Cast<AZFPCharacter>(SensedActor);
	if (!Player)
	{
		return;
	}
	if (PendingDialogueTarget == Player)
	{
		UpdatePendingDialogueTarget();
	}
	if (Player == SpottedCriminal)
	{
		ClearSpottedCriminal(Player); // #ZTODO: when should NPC forget about stolen items?
	}
}

void AZNPCAIController::ClearSpottedCriminal(AZFPCharacter* InSpottedCriminal)
{
	if (SpottedCriminal != InSpottedCriminal)
	{
		return;
	}
	// Pick first valid target as the next spotted criminal
	for (AActor* LookAtActor : SensedLookAtActors)
	{
		if (AZFPCharacter* NextPlayer = Cast<AZFPCharacter>(LookAtActor))
		{
			AZPlayerController* PC = NextPlayer->GetController<AZPlayerController>();
			if (StolenItems.Contains(PC) || DetectedBreakIns.Contains(PC))
			{
				SpottedCriminal = NextPlayer;
				SendStateTreeEvent(ZGameplayTags::AI_Dialogue_SpottedCriminalSet);
				return;
			}
		}
	}
	SpottedCriminal = nullptr;
	SendStateTreeEvent(ZGameplayTags::AI_Dialogue_SpottedCriminalCleared);
}

void AZNPCAIController::UpdatePendingDialogueTarget()
{
	if (SensedLookAtActors.Contains(PendingDialogueTarget))
	{
		AZPlayerController* PC = PendingDialogueTarget->GetController<AZPlayerController>();
		if (NPCharacter->GetFirstPendingDialogue(PC).DialogueID.IsValid())
		{
			return; // Current target is still valid
		}
	}
	AZCharacter* OldPendingDialogueTarget = PendingDialogueTarget;

    AZFPCharacter* BestTarget = nullptr;
    EZDialogueContext BestContext = EZDialogueContext::None;

    for (AActor* SensedActor : SensedLookAtActors)
    {
        if (AZFPCharacter* Player = Cast<AZFPCharacter>(SensedActor))
        {
            AZPlayerController* PC = Player->GetController<AZPlayerController>();
            FZPendingDialogue PendingDialogue = NPCharacter->GetFirstPendingDialogue(PC);
            if (PendingDialogue.DialogueID.IsValid() && PendingDialogue.DialogueContext >= BestContext)
            {
                BestTarget = Player;
                BestContext = PendingDialogue.DialogueContext;
            }
        }
    }

	if (OldPendingDialogueTarget != PendingDialogueTarget)
	{
		PendingDialogueTarget = BestTarget;
		SendStateTreeEvent(PendingDialogueTarget ? ZGameplayTags::AI_Dialogue_PendingTargetSet : ZGameplayTags::AI_Dialogue_PendingTargetCleared);
	}
}

void AZNPCAIController::PreSendStateTreeEvent(FGameplayTag EventTag, const FConstStructView Payload)
{
	if (PriorityStateEvents.HasTagExact(EventTag))
	{
		CachedStatePriority = GetCurrentHighestPriorityState();
	}
}

EZNPCStatePriority AZNPCAIController::GetCurrentHighestPriorityState() const
{
	for (uint8 Idx = 0; Idx < (static_cast<uint8>(EZNPCStatePriority::MAX)); ++Idx)
	{
		if (PriorityConditions[Idx]())
		{
			return static_cast<EZNPCStatePriority>(Idx);
		}
	}
	return EZNPCStatePriority::MAX;
}

