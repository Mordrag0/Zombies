// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ZAICharacter.h"
#include "ZInteractable.h"
#include "GameplayTagContainer.h"
#include "ZNPCharacter.generated.h"

class UZNavMoverComponent;
class AZWeapon;
class AZPath;
class AZInteractableBase;
class AZPlayerController;
struct FZPendingDialogue;
class AZBed;
class AZChair;
class UAIPerceptionComponent;
class UZInventoryItemData;
enum class EZDialogueContext : uint8;

UENUM()
enum class EZTemperament : uint8
{
	Neutral,
	Fearful,
	Aggressive,
};

USTRUCT()
struct FZPendingDialogue
{
	GENERATED_BODY()

	FZPendingDialogue(): DialogueID(FGameplayTag::EmptyTag), DialogueContext(static_cast<EZDialogueContext>(0)) {}
	FZPendingDialogue(FGameplayTag InDialogueID, EZDialogueContext InDialogueContext) 
		: DialogueID(InDialogueID), DialogueContext(InDialogueContext) {}

	FGameplayTag DialogueID;
	EZDialogueContext DialogueContext;
};

USTRUCT()
struct FZPendingDialogueList
{
	GENERATED_BODY()

	TArray<FZPendingDialogue> Dialogue;
};


DECLARE_MULTICAST_DELEGATE_OneParam(FZOnDialogueEnded, AZCharacter*);

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZNPCharacter : public AZAICharacter
{
	GENERATED_BODY()

public:
	AZNPCharacter();
	
	virtual void BeginPlay() override;
	
	virtual void ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult) override;
	
	virtual bool CanInteract(const AZCharacter* InCharacter) const override;
	virtual EZCharacterActivity GetCharacterActivity() const override;
	virtual FZInteractionParams GetInteractionParams(const AZCharacter* InCharacter) const override;

	void OnInteractionAttempted(AZCharacter* InCharacter);

	UFUNCTION(BlueprintPure)
	FName GetNPCName() const { return Name.GetTagLeafName(); }

	UFUNCTION(BlueprintPure)
	FGameplayTag GetFullNPCName() const { return Name; }

	bool CanTrade() const { return bCanTrade; }

	const UZInventoryItemData* PickBestWeapon() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const TSet<EZSkill>& GetTeachableSkills() const { return TeachableSkills; }

	const TArray<TObjectPtr<AActor>>& GetOwnedItems() const { return OwnedItems; }

	const FGameplayTagContainer& GetNPCGroups() const { return NPCGroups; }

	void SetLockedDialogueID(FGameplayTag DialogueId);

	FGameplayTag GetLockedDialogueID() const { return LockedDialogueID; }

	bool IsPendingDialogue(AZPlayerController* Player) const;

	bool IsPendingDialogue(AZPlayerController* Player, FGameplayTag DialogueID) const;

	void AddPendingDialogue(AZPlayerController* Player, FGameplayTag DialogueID, EZDialogueContext DialogueContext);

	FZPendingDialogue GetFirstPendingDialogue(AZPlayerController* Player) const;

	void RemovePendingDialogue(AZPlayerController* Player, FGameplayTag DialogueID);

	void SetDialogueContext(EZDialogueContext InContext);

	EZDialogueContext GetDialogueContext() const;

	void ClearDialogueContext();

	void CompleteContextDialogue(FGameplayTag EventID);

	const FGameplayTagContainer& GetContextCompletedEvents() const { return CompletedContextDialogues; }

	FGameplayTag GetDialogueContextInitiationID(EZDialogueContext Context) const;

	void Confront(AZPlayerController* Player, EZDialogueContext DialogueContext);

	virtual void GetAimViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	virtual void StartInteractionActivity(const FZInteractionState& InInteractionState) override;

	TScriptInterface<IZInteractable> GetAssignedInteractable(EZCharacterActivity Activity);

	EZTemperament GetTemperament() const {return Temperament; }

	int32 GetSleepStartTime() const {return SleepStartTime; }
	int32 GetSleepEndTime() const { return SleepEndTime; }

protected:
	virtual void Die(AController* DeathInstigator) override;
	
	virtual void StopActivity(EZCharacterActivity Activity) override;

	UAIPerceptionComponent* GetAIPerceptionComponent() const;

	virtual void SetSleeping(bool bSleeping) override;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Name;

	UPROPERTY(EditDefaultsOnly)
	bool bCanTrade;

	UPROPERTY(EditDefaultsOnly)
	bool bCanDie;

	UPROPERTY(EditDefaultsOnly)
	TSet<EZSkill> TeachableSkills;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer NPCGroups;

	UPROPERTY()
	TMap<TObjectPtr<AZPlayerController>, FZPendingDialogueList> PendingDialogues;

	FGameplayTag LockedDialogueID;

	UPROPERTY()
	EZDialogueContext DialogueContext;

	FGameplayTagContainer CompletedContextDialogues;

	UPROPERTY(EditDefaultsOnly)
	TMap<EZDialogueContext, FGameplayTag> DialogueContextInitiationIDs;

	UPROPERTY(EditInstanceOnly, meta = (ObjectMustImplement = "/Script/Zombies.ZInteractable"))
	TMap<EZCharacterActivity, TObjectPtr<AActor>> AssignedInteractables;

	UPROPERTY(EditDefaultsOnly)
	EZTemperament Temperament;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 SleepStartTime;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 SleepEndTime;

private:
	UPROPERTY(EditInstanceOnly, meta = (ObjectMustImplement = "/Script/Zombies.ZInteractable"))
	TArray<TObjectPtr<AActor>> OwnedItems;
};

