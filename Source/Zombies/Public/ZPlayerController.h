// Copyright 2026 Luka MarkuÃ…Â¡ All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "ZDelegateConnection.h"
#include "ZNetworkTypes.h"
#include "ZPlayerController.generated.h"

class UInputMappingContext;
class UZHUDWidget;
class AZCharacter;
class AZPlayerCameraManager;
class UZInventoryComponent;
class AZFPCharacter;
struct FZDialogueOptionRow;
class AZContainer;
class AZLock;
struct FInputActionValue;
class UInputAction;
class AZLockableBase;
class IZInteractable;
class AZNPCharacter;
class AZPlayerState;
struct FZInteractionState;
enum class EZNotificationType : uint8;
enum class EZSkill : uint8;
enum class EZCharacterActivity : uint32;

UENUM()
enum class EZInputMode : uint8
{
	None,
	Default,
	Lockpicking,
};

enum class EZPlayerActivity : uint8
{
	None,
	Lockpicking,
	Dialoguing,
	Looting,
};

USTRUCT()
struct FZMappingContextEntry
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputMappingContext> MappingContext;

    UPROPERTY(EditDefaultsOnly)
    int32 Priority = 0;
};

USTRUCT()
struct FZMappingContexts
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FZMappingContextEntry> MappingContexts;
};

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	AZPlayerController();

	UZHUDWidget* GetHUDWidget() const { return HUDWidget; }

	void ToggleInventory();

	void OnDialogueOptionSelected(const FZDialogueOptionRow* Row);
	void OnResponseClicked(const FZDialogueOptionRow* Row);
	void OnLearnSkillSelected(EZSkill Skill);

	void RefreshDialogueOptions();

	void ShowResponse(const FZDialogueOptionRow* Row);

	void Trade(AZNPCharacter* NPC);

	void StopLooting();

	void ToggleQuests();

	void ToggleSkills();

	void ToggleMap();

	void StopLockpicking();

	void Escape();

	void Unpause();

	void AddNotification(EZNotificationType NotificationType, const FText& Message);

	AZPlayerCameraManager* GetPlayerCameraManager() const;

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) { TeamID = InTeamID; }
	
	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const { return TeamID; }

	/** Retrieved owner attitude toward given Other object */
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const;

	void OnLockpickingFinished(bool bSuccess);

	void OnLootingFinished();

	void OnDialogueFinished();

	AZFPCharacter* GetFPCharacter() const { return FPCharacter; }

	virtual void InitPlayerState() override;

	virtual void OnRep_PlayerState() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void AcknowledgePossession(APawn* P) override;

	void InitHUD();
protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	void OnCharacterActivityStarted(const FZInteractionState& InteractionState);
	void OnCharacterActivityUpdated(const FZInteractionState& InteractionState);
	void OnCharacterActivityStopped(EZCharacterActivity Activity);
	
	void StartNPCInitiatedDialogue(AZNPCharacter* NPC, const FZInteractionParams& InteractionParams);
	void StartDialogue(AZNPCharacter* NPC, const FZInteractionParams& InteractionParams);
	void FinalizeDialogueStart(AZNPCharacter* NPC);
	void StartLooting(TScriptInterface<IZInteractable> InInteractionTarget, const FZInteractionParams& InteractionParams);
	void StartLockpicking(AZLockableBase* Target);

	void StopCurrentInteraction();

	void EndDialogue();

	void SetDefaultFOV(float FOV);

	UFUNCTION()
	void OnCharacterDeath();

	void FadeToBlack(float Duration);

	void LocalInit();

	void LocalDeinit();

	void CompleteDialogueEvent(FGameplayTag DialogueID, AZNPCharacter* NPC);

	UFUNCTION(Server, Reliable)
	void Server_Reliable_CompleteDialogueEvent(FGameplayTag DialogueID, AZNPCharacter* NPC);

	UFUNCTION(Server, Reliable)
	void Server_Reliable_UnlockLockpickTarget(AZLockableBase* LockpickTarget);

	UFUNCTION(Exec)
	void SaveGame(const FString& SlotName = TEXT("Main"));

	UFUNCTION(Exec)
	void LoadGame(const FString& SlotName = TEXT("Main"));

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT

	UFUNCTION(Exec)
	void Slomo(float Value);

	UFUNCTION(Exec)
	void SetTimeOfDay(float Time);

	UFUNCTION(Exec)
	void FreeXP(int32 XP);

	UFUNCTION(Exec)
	void SetVolume(float Volume);
	
	UFUNCTION(Exec)
	void ChangeReputation(int32 Faction, float ReputationChange);
#endif

	bool IsLockpicking() const { return PlayerActivity == EZPlayerActivity::Lockpicking; }
	
	void LockpickRotate(const FInputActionValue& Value);

	void LockpickTensionStart();
	void LockpickTensionEnd();

	void LockpickCancel();

	void SwitchMappingContexts(EZInputMode NewInputMode);

	void InitZPlayerState(AZPlayerState* PS);
	void CleanupZPlayerState(AZPlayerState* PS);

	void OnXPGained(int32 XP);
	void OnLevelUp(int32 Level);
	void OnSkillUnlocked(EZSkill Skill);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<EZInputMode, FZMappingContexts> MappingContexts;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UZHUDWidget> HUDWidget;

	UPROPERTY(EditDefaultsOnly)
	float DeathFadeDuration;

	FGenericTeamId TeamID;

	UPROPERTY()
	TObjectPtr<AZLock> Lock;

	UPROPERTY(EditDefaultsOnly, Category = "Lockpicking")
	TSubclassOf<AZLock> LockClass;

	UPROPERTY(EditDefaultsOnly, Category = "Lockpicking")
	float LockBlendTime;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LockpickRotateAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LockpickTensionAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LockpickCancelAction;

	EZInputMode CurrentInputMode;

	EZPlayerActivity PlayerActivity;

	UPROPERTY()
	TScriptInterface<IZInteractable> InteractableTarget; // Local player only

	UPROPERTY()
	TObjectPtr<AZFPCharacter> FPCharacter;

	FZDelegateConnection OnCharacterDeathConnection;
	FZDelegateConnection OnInteractionActivityStartedConnection;
	FZDelegateConnection OnInteractionActivityUpdatedConnection;
	FZDelegateConnection OnActivityStoppedConnection;

	FZDelegateConnection OnXPGainedConnection;
	FZDelegateConnection OnLevelUpConnection;
	FZDelegateConnection OnSkillUnlockedConnection;
};

