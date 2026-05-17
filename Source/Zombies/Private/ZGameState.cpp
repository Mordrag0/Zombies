// Copyright 2026 Luka Markuš. All rights reserved.


#include "ZGameState.h"
#include "Events/ZEvent.h"
#include "Characters/ZNPCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "ZPlayerController.h"
#include "AI/ZNPCAIController.h"
#include "Player/ZPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Components/ZLevelingComponent.h"
#include "Engine/DirectionalLight.h"
#include "AIController.h"
#include "ZGameplayStatics.h"
#include "AI/ZPath.h"
#include "Events/ZReaction.h"
#include "ZGameplayTags.h"
#include "StructUtils/InstancedStruct.h"
#include "AI/ZStateTreePayloads.h"
#include "UI/ZText.h"
#include "ZLog.h"
#include "Settings/ZGameSettings.h"

ETeamAttitude::Type FZFactionReputation::GetAttitudeTowardsPlayer() const
{
	return (Reputation < HostileThreshold) ? ETeamAttitude::Hostile : ((Reputation >= FriendlyThreshold) ? ETeamAttitude::Friendly : ETeamAttitude::Neutral);
}

AZGameState::AZGameState()
{
	PrimaryActorTick.bCanEverTick = true;

	TimeOfDay = 8.f;
	Day = 0;
	TimeScale = 3600.f / 24.f;
	DayOfTheYear = 100;
	Latitude = 45.f;
}

void AZGameState::CompleteEvent(FGameplayTag Event, AZCharacter* Character, AController* EventInstigator)
{
	ensure(HasAuthority());
	ensure(!CompletedEvents.Contains(Event));
	ensure(EventInstigator);
	const FZDialogueOptionRow* Row = GetDialogueOption(Event);
	if (Row && Row->bRepeatable) // Row->NPC might be a group
	{
		if (AZNPCharacter* NPC = Cast<AZNPCharacter>(Character))
		{
			if (AZNPCAIController* NPCController = NPCAIControllers.FindRef(NPC->GetFullNPCName()))
			{
				NPCController->OnDialogueEventCompleted(Row, Cast<AZPlayerController>(EventInstigator));
			}
		}
	}
	else
	{
		if (ZGameplayTags::IsCompletingTimedEvent(Event))
		{
			RemoveTimedEvent(Event.RequestDirectParent());
		}
		CompletedEvents.Add(Event);
		UpdateAvailableEvents(Event, EventInstigator);
		UpdateQuests(Event);
	}
}

void AZGameState::CompletePath(const AZPath* Path, AZNPCharacter* NPC, AController* EventInstigator)
{
	const FGameplayTag PathEventTag = Path->GetEventTag();
	if (PathEventTag.IsValid())
	{
		CompleteEvent(PathEventTag, NPC, EventInstigator);
		AZNPCAIController* NPCAIController = NPC->GetController<AZNPCAIController>();
		if (ensure(NPCAIController))
		{
			NPCAIController->CompletePath(Path->GetPathTag());
		}
	}
}

bool AZGameState::OptionBelongsToNPC(const FZDialogueOptionRow* DialogueOption, const AZNPCharacter* NPC) const
{
	if (DialogueOption->NPC == NPC->GetFullNPCName())
	{
		return true;
	}
	if (NPC->GetNPCGroups().HasTagExact(DialogueOption->NPC))
	{
		return true;
	}
	return false;
}

TArray<const FZDialogueOptionRow*> AZGameState::GetDialogueOptions(const FZDialogueParams& DialogueParams) const
{
	TArray<const FZDialogueOptionRow*> Ret;
	const TMap<FGameplayTag, const FZDialogueOptionRow*>& ContextOptions = DialogueOptions.FindRef(DialogueParams.DialogueContext);
	for (FGameplayTag DialogueID : DialogueParams.DialogueIDs)
	{
		if (const FZDialogueOptionRow* DialogueOption = ContextOptions.FindRef(DialogueID))
		{
			Ret.Add(DialogueOption);
		}
	}
	return Ret;
}

TArray<FGameplayTag> AZGameState::GetDialogueOptionIDs(AZPlayerController* PC, const AZNPCharacter* NPC) const
{
	const EZDialogueContext DialogueContext = NPC->GetDialogueContext();
	TArray<FGameplayTag> Ret;
	if (DialogueContext == EZDialogueContext::None)
	{
		const TMap<FGameplayTag, const FZDialogueOptionRow*>& ContextOptions = DialogueOptions.FindRef(EZDialogueContext::None);
		for (const FGameplayTag ID : AvailableEvents)
		{
			if (const FZDialogueOptionRow* DialogueOption = ContextOptions.FindRef(ID))
			{
				if (OptionBelongsToNPC(DialogueOption, NPC) && IsDialogueOptionAvailable(*DialogueOption, NPC) && AreDialogueOptionConditionsMet(*DialogueOption, PC))
				{
					Ret.Add(DialogueOption->ID);
				}
			}
		}
	}
	else
	{
		if (const TMap<FGameplayTag, const FZDialogueOptionRow*>* AllContextEvents = DialogueOptions.Find(DialogueContext))
		{
			for (const TPair<FGameplayTag, const FZDialogueOptionRow*>& ContextEvent : *AllContextEvents)
			{
				if (OptionBelongsToNPC(ContextEvent.Value, NPC) && IsOptionAvailable(*ContextEvent.Value, NPC->GetContextCompletedEvents()))
				{
					Ret.Add(ContextEvent.Value->ID);
				}
			}
		}
	}
	return Ret;
}

void AZGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;

    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CompletedQuests, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, OpenedQuests, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, FailedQuests, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, FactionReputation, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, TimeOfDay, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Day, Params);
}

AZWaypoint* AZGameState::GetWaypoint(FGameplayTag WaypointTag) const
{
	if (ensure(Waypoints.Contains(WaypointTag)))
	{
		return Waypoints.FindRef(WaypointTag);
	}
	UE_LOG(LogZGame, Error, TEXT("Failed to find waypoint %s"), *WaypointTag.ToString());
	return nullptr;
}

const FZDialogueOptionRow* AZGameState::GetDialogueOption(FGameplayTag EventID) const
{
	for (int32 Idx = 0; Idx < static_cast<uint8>(EZDialogueContext::MAX); ++Idx)
	{
		if (const FZDialogueOptionRow* DialogueOption = DialogueOptions[static_cast<EZDialogueContext>(Idx)].FindRef(EventID))
		{
			return DialogueOption;
		}
	}
	return nullptr;
}

void AZGameState::StartTimedEvent(FGameplayTag TimedEvent, int32 DurationHours, AZPlayerController* EventInstigator, AZNPCAIController* NPCAIController)
{
#if !UE_BUILD_SHIPPING
	for (const TPair<int32, FZTimedEventContextArray>& Pair : ActiveTimedEvents)
	{
		ensureMsgf(Pair.Value.EventContexts.IndexOfByPredicate([TimedEvent](const FZTimedEventContext& TimedEventContext)
		{
			return TimedEventContext.TimedEventTag == TimedEvent;
		}) == INDEX_NONE, TEXT("Tried to add the same timed event twice."));
	}
#endif
	const int32 TotalHour = GetTotalHour();
	ActiveTimedEvents.FindOrAdd(TotalHour + DurationHours).EventContexts.Add(FZTimedEventContext(TimedEvent, NPCAIController, EventInstigator));
}

void AZGameState::RemoveTimedEvent(FGameplayTag CompletedTimedEvent)
{
	const FGameplayTag TimedEvent = CompletedTimedEvent.RequestDirectParent();
	if (TimedEvent.IsValid())
	{
		int32 KeyToRemove = INDEX_NONE;
		for (TPair<int32, FZTimedEventContextArray>& Pair : ActiveTimedEvents)
		{
			Pair.Value.EventContexts.RemoveAll([TimedEvent](const FZTimedEventContext& TimedEventContext)
			{
				return TimedEventContext.TimedEventTag == TimedEvent;
			});
			if (Pair.Value.EventContexts.IsEmpty())
			{
				KeyToRemove = Pair.Key;
			}
		}
		if (KeyToRemove != INDEX_NONE)
		{
			ActiveTimedEvents.Remove(KeyToRemove);
		}
	}
	else
	{
		UE_LOG(LogZGame, Error, TEXT("Trying to remove invalid timed event: %s"), *CompletedTimedEvent.ToString());
	}
}

AZPath* AZGameState::GetPath(FGameplayTag PathTag) const
{
	if (ensure(Paths.Contains(PathTag)))
	{
		return Paths.FindRef(PathTag);
	}
	UE_LOG(LogZGame, Error, TEXT("Failed to find path %s"), *PathTag.ToString());
	return nullptr;
}

void AZGameState::ChangeReputation(EZFaction Faction, float Delta)
{
	if (FZFactionReputation* Reputation = GetFactionReputation(Faction))
	{
		const float OldReputation = Reputation->Reputation;
		Reputation->Reputation += Delta;
		UE_LOG(LogZGame, Log, TEXT("Reputation with %s changed from %f to %f"), *ENUM_TO_STRING(EZFaction, Faction), OldReputation, Reputation->Reputation);
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, FactionReputation, this);

		const ETeamAttitude::Type OldAttitude = FactionAttitudeTable[static_cast<int32>(Faction)][static_cast<int32>(EZFaction::Player)];
		const ETeamAttitude::Type NewAttitude = GetAttitudeTowardsPlayer(Faction);
		if (OldAttitude != NewAttitude)
		{
			UE_LOG(LogZGame, Log, TEXT("Attitude from faction %s towards the player changed from %s to %s"), 
				*ENUM_TO_STRING(EZFaction, Faction), *UZGameplayStatics::GetAttitudeAsString(OldAttitude), *UZGameplayStatics::GetAttitudeAsString(NewAttitude));
		}
		FactionAttitudeTable[static_cast<int32>(Faction)][static_cast<int32>(EZFaction::Player)] = NewAttitude;
		
		for (APlayerState* PS : PlayerArray)
		{
			AZPlayerController* PC = Cast<AZPlayerController>(PS->GetPlayerController());
			const FText NotificationText = FText::Format((Delta > 0 ? ZText::GainedReputation : ZText::LostReputation), FText::AsNumber(Delta), UZGameplayStatics::GetFactionText(Faction)); 
			PC->AddNotification(EZNotificationType::ReputationChange, NotificationText);
		}
	}
	else
	{
		UE_LOG(LogZGame, Error, TEXT("Faction reputation not set: %s"), *ENUM_TO_STRING(EZFaction, Faction));
	}
}

float AZGameState::GetReputation(EZFaction Faction) const
{
	if (const FZFactionReputation* Reputation = GetFactionReputation(Faction))
	{
		return Reputation->Reputation;
	}
	return 0.f;
}

ETeamAttitude::Type AZGameState::GetAttitudeTowardsPlayer(EZFaction Faction) const
{
	if (const FZFactionReputation* Reputation = GetFactionReputation(Faction))
	{
		return Reputation->GetAttitudeTowardsPlayer();
	}
	return ETeamAttitude::Neutral;
}

ETeamAttitude::Type AZGameState::GetAttitudeTowards(EZFaction From, EZFaction To) const
{
	return FactionAttitudeTable[static_cast<int>(From)][static_cast<int>(To)];
}

void AZGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
	{
		return;
	}

	SetTimeOfDay(TimeOfDay + (DeltaSeconds / TimeScale));
}

void AZGameState::SetTimeOfDay(float Time)
{
	const float PreviousTimeOfDay = TimeOfDay;
	TimeOfDay = FMath::Fmod(Time, 24.f);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, TimeOfDay, this);

	UpdateHour();

	if (PreviousTimeOfDay > TimeOfDay)
	{
		++Day;
		OnDayChanged.Broadcast(Day);
	}
	if (SunLight)
	{
		const FRotator SunRotation = CalculateSunAngle(TimeOfDay, Latitude, DayOfTheYear);
        SunLight->SetActorRotation(SunRotation);

		if (MoonLight)
		{
			const FRotator MoonRotation = FRotator(-SunRotation.Pitch, SunRotation.Yaw + 180.f, 0.f);
			MoonLight->SetActorRotation(MoonRotation);
		}
	}
	OnTimeChanged.Broadcast(TimeOfDay, Day);
}

void AZGameState::HandleBeginPlay()
{
	Init(); // Init game state before everything else

	Super::HandleBeginPlay();
}

void AZGameState::Init()
{
	PrimaryActorTick.SetTickFunctionEnable(HasAuthority());

	UpdateHour();

	for (int32 Idx = 0; Idx < static_cast<uint8>(EZDialogueContext::MAX); ++Idx)
	{
		DialogueOptions.Emplace(static_cast<EZDialogueContext>(Idx), TMap<FGameplayTag, const FZDialogueOptionRow*>());
	}
	if (ensure(EventsDataTable))
	{
		TArray<const FZEventRow*> AllEvents;
		EventsDataTable->GetAllRows(TEXT("AZGameState::BeginPlay"), AllEvents);
		for (const FZEventRow* Event : AllEvents)
		{
			AllOneTimeEvents.Add(Event->ID, Event);
			AllPossibleEvents.Add(Event->ID);
			for (const FGameplayTag RequiredEvent : Event->RequiredEvents)
			{
				RequiredByEvents.FindOrAdd(RequiredEvent).AddTag(Event->ID);
			}
			for (const FGameplayTag BlockedByEvent : Event->BlockedByEvents)
			{
				BlockedEvents.FindOrAdd(BlockedByEvent).AddTag(Event->ID);
			}
		}
	}
	if (ensure(DialogueDataTables.Num() > 0))
	{
		for (const UDataTable* DialogueDataTable : DialogueDataTables)
		{
			if (!DialogueDataTable)
			{
				continue;
			}
			TArray<const FZDialogueOptionRow*> AllDialogueOptions;
			DialogueDataTable->GetAllRows(TEXT("AZGameState::BeginPlay"), AllDialogueOptions);
			for (const FZDialogueOptionRow* DialogueOption : AllDialogueOptions)
			{
				if (DialogueOption->DialogueContext == EZDialogueContext::None)
				{
					ensure(!DialogueOption->bRepeatable); // Repeatable dialogue options require a dialogue context
				}
				DialogueOptions[DialogueOption->DialogueContext].Add(DialogueOption->ID, DialogueOption);
				if (!DialogueOption->bRepeatable)
				{
					AllOneTimeEvents.Add(DialogueOption->ID, DialogueOption);
					AllPossibleEvents.Add(DialogueOption->ID);
				}
				for (const FGameplayTag RequiredEvent : DialogueOption->RequiredEvents)
				{
					RequiredByEvents.FindOrAdd(RequiredEvent).AddTag(DialogueOption->ID);
				}
				for (const FGameplayTag BlockedByEvent : DialogueOption->BlockedByEvents)
				{
					BlockedEvents.FindOrAdd(BlockedByEvent).AddTag(DialogueOption->ID);
				}
			}
		}
	}

	InitAvailableEvents();

	if (ensure(QuestDataTable))
	{
		TArray<const FZQuestRow*> AllQuests;
		QuestDataTable->GetAllRows(TEXT("AZGameState::BeginPlay"), AllQuests);
		for (const FZQuestRow* Quest : AllQuests)
		{
			Quests.Add(Quest->ID, Quest);
		}
	}

	for (const TPair<FGameplayTag, const FZQuestRow*>& Quest : Quests)
    {
        // Index by required event
        QuestsByEvent.FindOrAdd(Quest.Value->RequiredEvent).Add(Quest.Value);
        
        // Index by each objective event
        for (const FZQuestObjective& Objective : Quest.Value->Objectives)
        {
            QuestsByEvent.FindOrAdd(Objective.RequiredEvent).Add(Quest.Value);
        }
        
        // Index by fail events
        for (FGameplayTag FailEvent : Quest.Value->FailEvents)
        {
            QuestsByEvent.FindOrAdd(FailEvent).Add(Quest.Value);
        }
    }

	for (int32 Idx = 0; Idx < static_cast<int>(EZFaction::MAX); Idx++)
	{
		for (int32 Idx2 = 0; Idx2 < static_cast<int>(EZFaction::MAX); Idx2++)
		{
			FactionAttitudeTable[Idx][Idx2] = (Idx == Idx2) // Same faction is always friendly
				? ETeamAttitude::Friendly
				: ((static_cast<EZFaction>(Idx) == EZFaction::Zombies) || (static_cast<EZFaction>(Idx2) == EZFaction::Zombies)) // If one of the factions are zombies they're hostile
				    ? ETeamAttitude::Hostile 
				    : ETeamAttitude::Neutral;
		}
	}
	for (const FZFactionPair& FactionPair : FactionAlliances)
	{
		FactionAttitudeTable[static_cast<int32>(FactionPair.FactionA)][static_cast<int32>(FactionPair.FactionB)] = ETeamAttitude::Friendly;
		FactionAttitudeTable[static_cast<int32>(FactionPair.FactionB)][static_cast<int32>(FactionPair.FactionA)] = ETeamAttitude::Friendly;
	}
	for (const FZFactionPair& FactionPair : FactionEnemies)
	{
		FactionAttitudeTable[static_cast<int32>(FactionPair.FactionA)][static_cast<int32>(FactionPair.FactionB)] = ETeamAttitude::Hostile;
		FactionAttitudeTable[static_cast<int32>(FactionPair.FactionB)][static_cast<int32>(FactionPair.FactionA)] = ETeamAttitude::Hostile;
	}

	ValidateEvents();
	bInitialized = true;
	
	NorthOffset = UZGameSettings::Get()->CompassNorthOffset;
}

void AZGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	AllOneTimeEvents.Empty();
	DialogueOptions.Empty();
	Quests.Empty();
	QuestsByEvent.Empty();
}

bool AZGameState::IsOptionAvailable(const FZEventRow& Row, const FGameplayTagContainer& CompletedContextEvents) const
{
	if (CompletedEvents.Contains(Row.ID) || CompletedContextEvents.HasTagExact(Row.ID)) // Is not completed yet
	{
		return false;
	}

	for (const FGameplayTag& RequiredEvent : Row.RequiredEvents) // If all required events completed
	{
		if (!CompletedEvents.Contains(RequiredEvent) && !CompletedContextEvents.HasTag(RequiredEvent))
		{
			return false;
		}
	}
	// If none of the events that row is blocked by are completed
	if (UZGameplayStatics::HasAnyExact(CompletedEvents, Row.BlockedByEvents) || CompletedContextEvents.HasAnyExact(Row.BlockedByEvents))
	{
		return false;
	}
	
	return true;
}

bool AZGameState::IsDialogueOptionAvailable(const FZDialogueOptionRow& Row, const AZNPCharacter* NPC) const
{
	// Skip calling IsOptionAvailable since we know that it will return true because the dialogue option is in AvailableEvents
	if (NPC)
	{
		if (Row.IsNPCInitiated())
		{
			return false;
		}
		const FGameplayTag LockedDialogueId = NPC->GetLockedDialogueID();
		if (LockedDialogueId.IsValid())
		{
			if (Row.ContinuationFrom != LockedDialogueId)
			{
				return false;
			}
		}
		else
		{
			if (Row.ContinuationFrom.IsValid())
			{
				return false;
			}
		}
	}
	return true;
}

bool AZGameState::AreDialogueOptionConditionsMet(const FZDialogueOptionRow& Row, const AZPlayerController* PC) const
{
	for (const TInstancedStruct<FZEventCondition>& Condition : Row.Conditions)
	{
		if (!Condition->IsMet(PC))
		{
			return false;
		}
	}
	return true;
}

void AZGameState::InitAvailableEvents()
{
	for (const TPair<FGameplayTag, const FZEventRow*> Event : AllOneTimeEvents)
	{
		if (Event.Value->RequiredEvents.Num() == 0)
		{
			AvailableEvents.Add(Event.Key);
		}
	}
}

void AZGameState::UpdateAvailableEvents(FGameplayTag CompletedEvent, AController* EventInstigator)
{
	ensure(HasAuthority());
	AllPossibleEvents.Remove(CompletedEvent);
	AvailableEvents.Remove(CompletedEvent);

	// Find events that just became unavailable
	FGameplayTagContainer NewlyUnavailableEvents;
	if (const FGameplayTagContainer* EventsThatCompletedEventBlocks = BlockedEvents.Find(CompletedEvent))
	{
		NewlyUnavailableEvents = UZGameplayStatics::FilterExact(AvailableEvents, *EventsThatCompletedEventBlocks);
		UZGameplayStatics::RemoveAll(AvailableEvents, NewlyUnavailableEvents);
		UZGameplayStatics::RemoveAll(AllPossibleEvents, *EventsThatCompletedEventBlocks); // Remove all blocked events even if they're not currently available
	}

	// Find events that just became available
	FGameplayTagContainer NewlyAvailableEvents;
	if (const FGameplayTagContainer* EventsThatRequireCompletedEvent = RequiredByEvents.Find(CompletedEvent))  // Get events that CompletedEvent is required by
	{
		for (const FGameplayTag EventThatRequiresCompletedEvent : *EventsThatRequireCompletedEvent)
		{
			if (AllPossibleEvents.Contains(EventThatRequiresCompletedEvent))
			{
				const FZEventRow* NewlyAvailableEventCandidateRow = AllOneTimeEvents[EventThatRequiresCompletedEvent];
				if (UZGameplayStatics::HasAllExact(CompletedEvents, NewlyAvailableEventCandidateRow->RequiredEvents)) // All required events completed
				{
					if (const FGameplayTagContainer* NewlyAvailableEventCandidateBlockedEvents = BlockedEvents.Find(EventThatRequiresCompletedEvent))
					{
						if (!UZGameplayStatics::HasAnyExact(CompletedEvents, *NewlyAvailableEventCandidateBlockedEvents))
						{
							NewlyAvailableEvents.AddTag(EventThatRequiresCompletedEvent); // No blocked events completed
						}
					}
					else
					{
						NewlyAvailableEvents.AddTag(EventThatRequiresCompletedEvent); // No blocked events
					}
				}
			}
		}
	}
	UZGameplayStatics::Append(AvailableEvents, NewlyAvailableEvents);
	
	OnEventCompleted(CompletedEvent, NewlyAvailableEvents, NewlyUnavailableEvents, EventInstigator);
}

void AZGameState::OnEventCompleted(FGameplayTag CompletedEvent, const FGameplayTagContainer& NewlyAvailableEvents, const FGameplayTagContainer& NewlyUnavailableEvents, AController* EventInstigator)
{
	AZPlayerController* PlayerEventInstigator = Cast<AZPlayerController>(EventInstigator);
	if (const FZEventRow* CompletedEventRow = AllOneTimeEvents.FindRef(CompletedEvent))
	{
		TObjectPtr<AZNPCAIController>* NPCAIController = NPCAIControllers.Find(CompletedEventRow->NPC);
		if (ensure(NPCAIController))
		{
			for (const TInstancedStruct<FZEventReaction>& EventReaction : CompletedEventRow->OnCompleted)
			{
				EventReaction->Execute(CompletedEventRow, PlayerEventInstigator, *NPCAIController);
			}
		}
	}
	if (const FZDialogueOptionRow* DialogueOptionRow = GetDialogueOption(CompletedEvent))
	{
		TObjectPtr<AZNPCAIController>* NPCAIController = NPCAIControllers.Find(DialogueOptionRow->NPC);
		if (ensure(NPCAIController))
		{
			(*NPCAIController)->OnDialogueEventCompleted(DialogueOptionRow, PlayerEventInstigator);
		}
	}
	for (const FGameplayTag AvailableEvent : NewlyAvailableEvents)
	{
		const FZEventRow* AvailableEventRow = AllOneTimeEvents.FindRef(AvailableEvent);
		if (ensure(AvailableEventRow))
		{
			TObjectPtr<AZNPCAIController>* NPCAIController = NPCAIControllers.Find(AvailableEventRow->NPC);
			if (ensure(NPCAIController))
			{
				for (const TInstancedStruct<FZEventReaction>& EventReaction : AvailableEventRow->OnAvailable)
				{
					EventReaction->Execute(AvailableEventRow, PlayerEventInstigator, *NPCAIController);
				}
			}
		}
		if (const FZDialogueOptionRow* DialogueOptionRow = GetDialogueOption(AvailableEvent))
		{
			TObjectPtr<AZNPCAIController>* NPCAIController = NPCAIControllers.Find(DialogueOptionRow->NPC);
			if (ensure(NPCAIController))
			{
				(*NPCAIController)->OnDialogueEventAvailable(DialogueOptionRow, PlayerEventInstigator);
			}
		}
	}
	for (const FGameplayTag UnavailableEvent : NewlyUnavailableEvents)
	{
		const FZEventRow* UnavailableEventRow = AllOneTimeEvents.FindRef(UnavailableEvent);
		if (ensure(UnavailableEventRow))
		{
			TObjectPtr<AZNPCAIController>* NPCAIController = NPCAIControllers.Find(UnavailableEventRow->NPC);
			if (ensure(NPCAIController))
			{
				for (const TInstancedStruct<FZEventReaction>& EventReaction : UnavailableEventRow->OnUnavailable)
				{
					EventReaction->Execute(UnavailableEventRow, PlayerEventInstigator, *NPCAIController);
				}
			}
		}
		if (const FZDialogueOptionRow* DialogueOptionRow = GetDialogueOption(UnavailableEvent))
		{
			TObjectPtr<AZNPCAIController>* NPCAIController = NPCAIControllers.Find(DialogueOptionRow->NPC);
			if (ensure(NPCAIController))
			{
				(*NPCAIController)->OnDialogueEventUnavailable(DialogueOptionRow, PlayerEventInstigator);
			}
		}
	}
}

void AZGameState::UpdateQuests(FGameplayTag CompletedEvent)
{
    const TSet<const FZQuestRow*>* RelevantQuests = QuestsByEvent.Find(CompletedEvent);
	if (!RelevantQuests)
	{
		return;
	}

    for (const FZQuestRow* Quest : *RelevantQuests)
    {
		if (CompletedQuests.HasTagExact(Quest->ID))
		{
			continue;
		}
		else if (FailedQuests.HasTagExact(Quest->ID))
		{
			continue;
		}
		else if (UnavailableQuests.HasTagExact(Quest->ID))
		{
			continue;
		}
		else if (OpenedQuests.HasTagExact(Quest->ID))
		{
			if (Quest->RequiresObjective(CompletedEvent))
			{
				if (Quest->AreAllObjectivesCompleted(CompletedEvents))
				{
					// Complete quest
					OpenedQuests.RemoveTag(Quest->ID);
					CompletedQuests.AddTag(Quest->ID);
					OnQuestCompleted(Quest);
				}
			}
			else if (Quest->FailEvents.HasTagExact(CompletedEvent))
			{
				// Fail quest
				OpenedQuests.RemoveTag(Quest->ID);
				FailedQuests.AddTag(Quest->ID);
			}
		}
		else
		{
			if (Quest->RequiredEvent == CompletedEvent)
			{
				// Open quest
				OpenedQuests.AddTag(Quest->ID);
			}
			else if (Quest->FailEvents.HasTagExact(CompletedEvent))
			{
				// Make quest unavailable
				UnavailableQuests.AddTag(Quest->ID);
			}
		}
	}
}

void AZGameState::OnQuestCompleted(const FZQuestRow* Quest)
{
	for (const TPair<EZFaction, float>& ReputationChange : Quest->ReputationChanges)
	{
		ChangeReputation(ReputationChange.Key, ReputationChange.Value);
	}
	for (APlayerState* PS : PlayerArray)
	{
		if (AZPlayerState* ZPS = Cast<AZPlayerState>(PS))
		{
			ZPS->GetLevelingComponent()->AddXP(Quest->XPReward);
		}
	}
}

FZFactionReputation* AZGameState::GetFactionReputation(EZFaction Faction)
{
	for (FZFactionReputation& Reputation : FactionReputation)
	{
		if (Reputation.Faction == Faction)
		{
			return &Reputation;
		}
	}
	return nullptr;
}

const FZFactionReputation* AZGameState::GetFactionReputation(EZFaction Faction) const
{
	for (const FZFactionReputation& Reputation : FactionReputation)
	{
		if (Reputation.Faction == Faction)
		{
			return &Reputation;
		}
	}
	return nullptr;
}

FRotator AZGameState::CalculateSunAngle(float InTimeOfDay, float InLatitude, float InDayOfYear) const
{
	const float LatRad = FMath::DegreesToRadians(InLatitude);
	// Solar declination - how far the sun is north/south of equator
	const float Declination = FMath::DegreesToRadians(23.45f * FMath::Sin(FMath::DegreesToRadians((360.f / 365.f) * (InDayOfYear - 81.f))));
	// Hour angle - 0 at noon, negative in morning, positive in afternoon
	const float HourAngle = FMath::DegreesToRadians((InTimeOfDay - 12.f) * 15.f);
	// Solar altitude - how high the sun is above the horizon
	const float SinAltitude = FMath::Sin(LatRad) * FMath::Sin(Declination) + FMath::Cos(LatRad) * FMath::Cos(Declination) * FMath::Cos(HourAngle);
	const float Altitude = FMath::Asin(SinAltitude);
	// Solar azimuth - compass direction of the sun
	const float SinAzimuth = -FMath::Cos(Declination) * FMath::Sin(HourAngle);
	const float CosAzimuth = FMath::Cos(LatRad) * FMath::Sin(Declination) - FMath::Sin(LatRad) * FMath::Cos(Declination) * FMath::Cos(HourAngle);
	const float Azimuth = FMath::RadiansToDegrees(FMath::Atan2(SinAzimuth, CosAzimuth));
	// Convert altitude and azimuth to directional light pitch and yaw
	const float Pitch = -FMath::RadiansToDegrees(Altitude);
	const float Yaw = Azimuth + NorthOffset;

	return FRotator(Pitch, Yaw, 0.f);
}

void AZGameState::UpdateHour()
{
	const float PreviousHour = Hour;
	Hour = FMath::FloorToInt(TimeOfDay);
	if (PreviousHour != Hour)
	{
		HandleHourChanged(PreviousHour);
	}
}

void AZGameState::HandleHourChanged(int32 PreviousHour)
{
	for (int32 Idx = PreviousHour + 1; Idx <= Hour; Idx++)
	{
		FZTimedEventContextArray NewlyExpiredEvents;
		if (ActiveTimedEvents.RemoveAndCopyValue(Idx, NewlyExpiredEvents))
		{
			for (const FZTimedEventContext& NewlyExpiredTimedEvent : NewlyExpiredEvents.EventContexts)
			{
				OnTimedEventExpired(NewlyExpiredTimedEvent);
			}
		}
	}
	for (const TPair<FGameplayTag, TObjectPtr<AZNPCAIController>>& KVP : NPCAIControllers)
	{
		//const FInstancedStruct Payload = FInstancedStruct::Make(FZHourChangedPayload(Hour, KVP.Value->GetCachedDesiredActivity()));
		//KVP.Value->SendStateTreeEvent(ZGameplayTags::AI_Time_HourChanged, Payload);
		KVP.Value->HandleHourChanged(Hour);
	}
}

int32 AZGameState::GetTotalHour() const
{
	return Day * 24 + Hour;
}

void AZGameState::OnTimedEventExpired(const FZTimedEventContext& ExpiredTimedEvent)
{
	const FGameplayTag ExpiredTimedEventExpiredId = ZGameplayTags::GetFullTag(ExpiredTimedEvent.TimedEventTag, ZGameplayTags::TimedEventExpiredLeafName);
	if (ExpiredTimedEventExpiredId.IsValid())
	{
		CompleteEvent(ExpiredTimedEventExpiredId, ExpiredTimedEvent.NPCAIController->GetNPCharacter(), ExpiredTimedEvent.EventInstigator);
	}
	else
	{
		UE_LOG(LogZGame, Error, TEXT("Failed to find expired tag for timed event %s"), *ExpiredTimedEvent.TimedEventTag.ToString());
	}
}

void AZGameState::ValidateEvents()
{
#if WITH_EDITOR
	
#endif
}

void AZGameState::OnRep_FactionReputation()
{
	// #ZTODO
}

void AZGameState::OnRep_TimeOfDay()
{
	UpdateHour();
	OnTimeChanged.Broadcast(TimeOfDay, Day);
}

void AZGameState::OnRep_Day()
{
	OnTimeChanged.Broadcast(TimeOfDay, Day);
}

