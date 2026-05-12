// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameplayTagContainer.h"
#include "Events/ZEvent.h"
#include "Events/ZQuest.h"
#include "ZTypes.h"
#include "ZGameState.generated.h"

class AZNPCharacter;
class AZPlayerController;
class AZNPCAIController;
class ADirectionalLight;
class AAIController;
class AController;
class AZWaypoint;
class AZPath;
class AZCharacter;
struct FZPathReactionRow;
struct FZHomeTransformReactionRow;
struct FZSimpleReactionRow;
struct FZDialogueParams;
enum class EZDialogueContext : uint8;
enum class EZFaction : uint8;
namespace ETeamAttitude { enum Type : int; }

DECLARE_MULTICAST_DELEGATE_OneParam(FZOnDayChanged, int32)
DECLARE_MULTICAST_DELEGATE_TwoParams(FZOnTimeChanged, float, int32)

USTRUCT()
struct FZFactionReputation
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EZFaction Faction = EZFaction::NoFaction;

	UPROPERTY(EditDefaultsOnly)
	float Reputation = 0.f;

    UPROPERTY(EditDefaultsOnly)
    float HostileThreshold = -100.f;

    UPROPERTY(EditDefaultsOnly)
    float FriendlyThreshold = 100.f;

	ETeamAttitude::Type GetAttitudeTowardsPlayer() const;
};

USTRUCT()
struct FZFactionPair
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    EZFaction FactionA = EZFaction::NoFaction;

    UPROPERTY(EditDefaultsOnly)
    EZFaction FactionB = EZFaction::NoFaction;
};

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AZGameState();

	void CompleteEvent(FGameplayTag Event, AZCharacter* Character, AController* EventInstigator);

	void CompletePath(const AZPath* Path, AZNPCharacter* NPC, AController* EventInstigator);

	bool OptionBelongsToNPC(const FZDialogueOptionRow* DialogueOption, const AZNPCharacter* NPC) const;
	
	TArray<const FZDialogueOptionRow*> GetDialogueOptions(const FZDialogueParams& DialogueParams) const;
	TArray<FGameplayTag> GetDialogueOptionIDs(AZPlayerController* PC, const AZNPCharacter* NPC) const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	const FZDialogueOptionRow* GetDialogueOption(FGameplayTag EventID) const;

	AZWaypoint* GetWaypoint(FGameplayTag WaypointTag) const;

	AZPath* GetPath(FGameplayTag PathTag) const;

	const FZQuestRow* GetQuest(FGameplayTag QuestID) const { return Quests.FindRef(QuestID); }

	const FGameplayTagContainer& GetCompletedQuests() const {return CompletedQuests;}

	const FGameplayTagContainer& GetOpenedQuests() const {return OpenedQuests;}

	const FGameplayTagContainer& GetFailedQuests() const {return FailedQuests;}

	const FGameplayTagContainer& GetUnavailableQuests() const {return UnavailableQuests;}
	
	void ChangeReputation(EZFaction Faction, float Delta);

	float GetReputation(EZFaction Faction) const;

	ETeamAttitude::Type GetAttitudeTowardsPlayer(EZFaction Faction) const;

	ETeamAttitude::Type GetAttitudeTowards(EZFaction From, EZFaction To) const;
	
	virtual void Tick(float DeltaSeconds) override;

	void SetSunLight(ADirectionalLight* InSunLight) { SunLight = InSunLight; }

	void SetMoonLight(ADirectionalLight* InMoonLight) { MoonLight = InMoonLight; }

	void SetTimeOfDay(float Time);

	float GetTimeOfDay() const { return TimeOfDay; }

	int32 GetDay() const { return Day; }

	int32 GetHour() const { return Hour; }

	template<typename T>
	void RegisterActor(TMap<FGameplayTag, TObjectPtr<T>>& Map, T* Actor, FGameplayTag Tag);

	template<typename T>
	void UnregisterActor(TMap<FGameplayTag, TObjectPtr<T>>& Map, FGameplayTag Tag);

	TMap<FGameplayTag, TObjectPtr<AZNPCAIController>>& GetNPCAIControllers() { return NPCAIControllers; }

	TMap<FGameplayTag, TObjectPtr<AZWaypoint>>& GetWaypoints() { return Waypoints; }

	TMap<FGameplayTag, TObjectPtr<AZPath>>& GetPaths() { return Paths; }
	
	virtual void HandleBeginPlay();

	bool GetInitialized() const { return bInitialized; }

	FZOnDayChanged OnDayChanged;
	FZOnTimeChanged OnTimeChanged;

protected:
	void Init();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool IsOptionAvailable(const FZEventRow& Row, const FGameplayTagContainer& CompletedContextEvents) const;

	bool IsDialogueOptionAvailable(const FZDialogueOptionRow& Row, const AZNPCharacter* NPC) const;

	bool AreDialogueOptionConditionsMet(const FZDialogueOptionRow& Row, const AZPlayerController* PC) const;
	
	void InitAvailableEvents();

	void UpdateAvailableEvents(FGameplayTag CompletedEvent, AController* EventInstigator);

	void OnEventCompleted(FGameplayTag CompletedEvent, const FGameplayTagContainer& NewlyAvailableEvents, const FGameplayTagContainer& NewlyUnavailableEvents, AController* EventInstigator);

	void UpdateQuests(FGameplayTag CompletedEvent);

	void OnQuestCompleted(const FZQuestRow* Quest);

	FZFactionReputation* GetFactionReputation(EZFaction Faction);
	const FZFactionReputation* GetFactionReputation(EZFaction Faction) const;

	FRotator CalculateSunAngle(float InTimeOfDay, float InLatitude, float InDayOfYear) const;

	void UpdateHour();

	void HandleHourChanged();

	void ValidateEvents();
	
	TMap<FGameplayTag, const FZEventRow*> AllOneTimeEvents;

	TMap<EZDialogueContext, TMap<FGameplayTag, const FZDialogueOptionRow*>> DialogueOptions;

	TMap<FGameplayTag, const FZQuestRow*> Quests;
	
	TMap<FGameplayTag, TSet<const FZQuestRow*>> QuestsByEvent;

	UPROPERTY(EditDefaultsOnly, Category = "ZEvents")
	TArray<TObjectPtr<UDataTable>> DialogueDataTables;

	UPROPERTY(EditDefaultsOnly, Category = "ZEvents")
	TObjectPtr<UDataTable> EventsDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "ZEvents")
	TObjectPtr<UDataTable> QuestDataTable;

	UPROPERTY(Replicated)
	FGameplayTagContainer CompletedEvents;

	FGameplayTagContainer AvailableEvents;

	UPROPERTY(Replicated)
	FGameplayTagContainer CompletedQuests;

	UPROPERTY(Replicated)
	FGameplayTagContainer OpenedQuests;

	UPROPERTY(Replicated)
	FGameplayTagContainer FailedQuests;

	UPROPERTY()
	FGameplayTagContainer UnavailableQuests;

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_FactionReputation)
	TArray<FZFactionReputation> FactionReputation;

	UFUNCTION()
	void OnRep_FactionReputation();

	ETeamAttitude::Type FactionAttitudeTable[static_cast<uint8>(EZFaction::MAX)][static_cast<uint8>(EZFaction::MAX)]; // #ZTODO: SaveGame

	UPROPERTY(EditDefaultsOnly)
	TArray<FZFactionPair> FactionAlliances;

	UPROPERTY(EditDefaultsOnly)
	TArray<FZFactionPair> FactionEnemies;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<AZNPCAIController>> NPCAIControllers;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<AZWaypoint>> Waypoints;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<AZPath>> Paths;

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_TimeOfDay, Category = "Time of day")
	float TimeOfDay;
	
	UFUNCTION()
	void OnRep_TimeOfDay();

	UPROPERTY(ReplicatedUsing = OnRep_Day)
	int32 Day;

	UFUNCTION()
	void OnRep_Day();

	int32 Hour;
	
	UPROPERTY(EditDefaultsOnly, Category = "Time of day")
	float TimeScale; // How many real seconds per game hour
	
	UPROPERTY(EditDefaultsOnly, Category = "Time of day")
	float DayOfTheYear;

	UPROPERTY()
	TObjectPtr<ADirectionalLight> SunLight;

	UPROPERTY()
	TObjectPtr<ADirectionalLight> MoonLight;

	UPROPERTY(EditDefaultsOnly, Category = "Time of Day")
	float Latitude; // Degrees, 0 = equator, 90 = north pole

	bool bInitialized;

	friend class UZSaveSubsystem;
};

template<typename T>
void AZGameState::RegisterActor(TMap<FGameplayTag, TObjectPtr<T>>& Map, T* Actor, FGameplayTag Tag)
{
	ensureMsgf(!Map.Contains(Tag), TEXT("Duplicate registration for tag %s"), *Tag.ToString());
	Map.Add(Tag, Actor);
}

template<typename T>
void AZGameState::UnregisterActor(TMap<FGameplayTag, TObjectPtr<T>>& Map, FGameplayTag Tag)
{
	ensureMsgf(Map.Contains(Tag), TEXT("Trying to unregister unknown tag %s"), *Tag.ToString());
	Map.Remove(Tag);
}

