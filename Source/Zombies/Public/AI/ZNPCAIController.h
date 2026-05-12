// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZAIController.h"
#include "GameplayTagContainer.h"
#include "ZDelegateConnection.h"
#include "ZNPCAIController.generated.h"

class AZNPCharacter;
struct FZDialogueOptionRow;
class AZPlayerController;
class AZLockableBase;
class AZInteractableBase;
struct FZEventRow;
class AZFPCharacter;
class IZInteractable;
class AZPath;
enum class EZCharacterActivity : uint32;

using FZPriorityCondition = TFunction<bool()>;

UENUM()
enum class EZNPCStatePriority : uint8
{
	Combat,
	ConfrontCriminal,
	TalkToPlayer,
	FollowPath,
	Activity,
	MAX
};

struct FZNPCPriorityStateEntry
{
	EZNPCStatePriority Priority;
	FZPriorityCondition Condition;
	FGameplayTagContainer RelevantEvents;
};

USTRUCT()
struct FZStolenItems
{
	GENERATED_BODY()

	TMap<FPrimaryAssetId, int32> Items;
};

USTRUCT()
struct FZPathData
{
	GENERATED_BODY()

	FZPathData()
	{
		Reset();
	}

	FZPathData(FGameplayTag InPathTag, AZNPCharacter* InNPCharacter, AZFPCharacter* InPlayer, bool bInEscortPlayer);

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FGameplayTag PathTag;

	UPROPERTY()
	TObjectPtr<AZNPCharacter> NPCharacter;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AZPath> Path;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AZFPCharacter> Player;
	
	bool bEscortPlayer;

	void Reset()
	{
		PathTag = FGameplayTag::EmptyTag;
		Path = nullptr;
		WaypointIndex = -1;
		Player = nullptr;
	}

	bool IsValid() const { return PathTag.IsValid(); }
	bool IncrementWaypointIndex();
	int32 GetWaypointIndex() const { return WaypointIndex; }
	bool IsComplete() const;

private:
	int32 WaypointIndex;
};

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZNPCAIController : public AZAIController
{
	GENERATED_BODY()

public:
	AZNPCAIController();

	virtual void PostInitializeComponents() override;

	AZNPCharacter* GetNPCharacter() const { return NPCharacter; }

	void OnDialogueEventCompleted(const FZDialogueOptionRow* DialogueOption, AZPlayerController* EventInstigator);
	void OnDialogueEventAvailable(const FZDialogueOptionRow* DialogueOption, AZPlayerController* EventInstigator);
	void OnDialogueEventUnavailable(const FZDialogueOptionRow* DialogueOption, AZPlayerController* EventInstigator);

	EZCharacterActivity GetDesiredActivity(int32 Hour) const;
	EZCharacterActivity GetCachedDesiredActivity() const { return CachedDesiredActivity; }
	
	void Attack(AZPlayerController* PC);
	void TakeBackStolenItems(AZPlayerController* PC);
	void WarnAboutBreakingIn(AZPlayerController* PC);

	FZPathData& GetPathData() { return PathData; }
	const FZPathData& GetPathData() const { return PathData; }
	
	void StartPath(FGameplayTag PathTag, AZPlayerController* EventInstigator, bool bEscortPlayer);
	void CancelPath(FGameplayTag PathTag);
	void CompletePath(FGameplayTag PathTag);
	
	bool CanEnterPriorityState(EZNPCStatePriority Priority) const;
	bool IsHighestPriorityState(EZNPCStatePriority Priority) const;

	void HandleHourChanged(int32 Hour);
	
	void SetHomeTransform(FGameplayTag WaypointTag);

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	void OnInteractionStarted(TScriptInterface<IZInteractable> NPC, TScriptInterface<IZInteractable> Player);
	void OnInteractionStopped(TScriptInterface<IZInteractable> NPC, TScriptInterface<IZInteractable> Player);

	void OnOwnedItemInteractionStarted(TScriptInterface<IZInteractable> OwnedItem, TScriptInterface<IZInteractable> InInteractingCharacter);

	virtual void OnSensedLookAtActorAdded(AActor* SensedActor) override;

	virtual void OnSensedLookAtActorRemoved(AActor* SensedActor) override;

	void ClearSpottedCriminal(AZFPCharacter* InSpottedCriminal);

	void UpdatePendingDialogueTarget();

	virtual void RegisterPriorityStates();

	virtual void PreSendStateTreeEvent(FGameplayTag EventTag, const FConstStructView Payload = FConstStructView()) override;
	
	EZNPCStatePriority GetCurrentHighestPriorityState() const;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector HomeLocation;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FRotator HomeRotation;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AZCharacter> InteractingCharacter;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AZFPCharacter> PendingDialogueTarget;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AZFPCharacter> SpottedCriminal;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FZPathData PathData;

	UPROPERTY()
	TObjectPtr<AZNPCharacter> NPCharacter;

	UPROPERTY()
	TMap<TObjectPtr<AZPlayerController>, FZStolenItems> StolenItems;

	UPROPERTY()
	TMap<TObjectPtr<AZPlayerController>, TObjectPtr<AZLockableBase>> DetectedBreakIns;

	FZDelegateConnection OnInteractionStartedConnection;
	FZDelegateConnection OnInteractionStoppedConnection;

	TMap<TObjectPtr<AActor>, FZDelegateConnection> OnOwnedItemsInteractionStartedConnections;

	EZNPCStatePriority CachedStatePriority;
	EZCharacterActivity CachedDesiredActivity;

private:
	FZPriorityCondition PriorityConditions[static_cast<uint8>(EZNPCStatePriority::MAX)];

	FGameplayTagContainer PriorityStateEvents;
};

