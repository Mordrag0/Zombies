// Copyright 2026 Luka MarkuÃ…Â¡ All rights reserved.


#include "ZPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "UI/ZHUDWidget.h"
#include "Characters/ZCharacter.h"
#include "Characters/ZNPCharacter.h"
#include "Characters/ZFPCharacter.h"
#include "Player/ZPlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Events/ZEvent.h"
#include "ZGameState.h"
#include "SaveLoad/ZSaveSubsystem.h"
#include "Misc/ZLock.h"
#include "EnhancedInputComponent.h"
#include "Misc/ZLockableBase.h"
#include "Player/ZPlayerState.h"
#include "Components/ZLevelingComponent.h"
#include "Components/ZSkillsComponent.h"
#include "ZTypes.h"
#include "AudioDevice.h"
#include "ZGameInstance.h"
#include "ZLog.h"
#include "UI/ZHUD.h"
#include "UI/ZText.h"

#define LOCTEXT_NAMESPACE "Zombies"

AZPlayerController::AZPlayerController()
{
	DeathFadeDuration = 3.f;

	LockBlendTime = .5f;

	CurrentInputMode = EZInputMode::None;
}

void AZPlayerController::ToggleInventory()
{
	if (HUDWidget && FPCharacter)
	{
		HUDWidget->ShowInventory(FPCharacter->GetInventoryComponent());
	}
}

void AZPlayerController::StartNPCInitiatedDialogue(AZNPCharacter* NPC, const FZInteractionParams& InteractionParams)
{
	if (!ensure(IsLocalPlayerController()))
	{
		return;
	}
	if (!ensure(NPC))
	{
		return;
	}
	if (!ensure(!InteractableTarget || InteractableTarget == NPC)) // #ZTODO fix NPC trying to talk to you while you're already talking to someone else
	{
		return;
	}
	if (HUDWidget)
	{
		AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
		if (!ensure(GS))
		{
			return;
		}
		const FZNPCDialogueParams& NPCDialogueParams = InteractionParams.Get<FZNPCDialogueParams>();
		const FZDialogueOptionRow* NPCInitiatedRow = GS->GetDialogueOption(NPCDialogueParams.PendingDialogueID);
		if (NPCInitiatedRow)
		{
			HUDWidget->StartDialogue(NPCInitiatedRow);
		}
		FinalizeDialogueStart(NPC);
	}
}

void AZPlayerController::StartDialogue(AZNPCharacter* NPC, const FZInteractionParams& InteractionParams)
{
	if (!ensure(IsLocalPlayerController()))
	{
		return;
	}
	if (!ensure(NPC))
	{
		return;
	}
	if (InteractableTarget)
	{
		ensure(0);
		return;
	}
	if (HUDWidget)
	{
		const FZDialogueParams& DialogueParams = InteractionParams.Get<FZDialogueParams>();
		HUDWidget->StartDialogue(NPC, DialogueParams);
		FinalizeDialogueStart(NPC);
	}
}

void AZPlayerController::FinalizeDialogueStart(AZNPCharacter* NPC)
{
	InteractableTarget = NPC;
	PlayerActivity = EZPlayerActivity::Dialoguing;
}

void AZPlayerController::OnDialogueOptionSelected(const FZDialogueOptionRow* Row)
{
	CompleteDialogueEvent(Row->ID, Cast<AZNPCharacter>(InteractableTarget.GetObject()));
	if (Row->DoesNPCRespond())
	{
		ShowResponse(Row);
	}
	else
	{
		if (Row->bEndsDialogue)
		{
			OnDialogueFinished();
		}
		else
		{
			RefreshDialogueOptions();
		}
	}
}

void AZPlayerController::OnResponseClicked(const FZDialogueOptionRow* Row)
{
	if (Row->IsNPCInitiated())
	{
		CompleteDialogueEvent(Row->ID, Cast<AZNPCharacter>(InteractableTarget.GetObject()));
	}
	if (Row->bEndsDialogue)
	{
		OnDialogueFinished();
	}
	else if (!Row->IsNPCInitiated()) // If it's NPC initiated then CompleteDialogueEvent will refresh dialogue options, calling RefreshDialogueOptions again would clear the dialogue widget and set it to wait state
	{
		RefreshDialogueOptions();
	}
}

void AZPlayerController::OnLearnSkillSelected(EZSkill Skill)
{
	if (AZPlayerState* PS = GetPlayerState<AZPlayerState>())
	{
		const bool bLearnedSkill = PS->Learn(Skill);
		if (bLearnedSkill)
		{
			RefreshDialogueOptions();
		}
		else
		{
			// #ZTODO notify failed to learn new skill
			UE_LOG(LogZombies, Error, TEXT("Failed to learn a new skill: %s"), *ENUM_TO_STRING(EZSkill, Skill));
		}
	}
}

void AZPlayerController::RefreshDialogueOptions()
{
	if (HUDWidget)
	{
		HUDWidget->RefreshDialogueOptions(); 
	}
}

void AZPlayerController::ShowResponse(const FZDialogueOptionRow* Row)
{
	if (HUDWidget)
	{
		HUDWidget->ShowResponse(Row);
	}
}

void AZPlayerController::EndDialogue()
{
	if (HUDWidget)
	{
		HUDWidget->EndDialogue();
	}

	InteractableTarget = nullptr;
	PlayerActivity = EZPlayerActivity::None;
}

void AZPlayerController::Trade(AZNPCharacter* NPC)
{
	if (HUDWidget)
	{
		HUDWidget->StartTrade(NPC);
	}
}

void AZPlayerController::StartLooting(TScriptInterface<IZInteractable> InInteractionTarget, const FZInteractionParams& InteractionParams)
{
	ensure(!InteractableTarget || InteractableTarget == InInteractionTarget);

	InteractableTarget = InInteractionTarget;
	PlayerActivity = EZPlayerActivity::Looting;
	UZInventoryComponent* Inventory = InteractionParams.Get<FZLootingParams>().InventoryComponent;
	if (HUDWidget)
	{
		HUDWidget->StartLooting(Inventory);
	}
}

void AZPlayerController::StopLooting()
{
	if (HUDWidget)
	{
		HUDWidget->StopLooting();
	}

	InteractableTarget = nullptr;
	PlayerActivity = EZPlayerActivity::None;
}

void AZPlayerController::ToggleQuests()
{
	if (HUDWidget)
	{
		HUDWidget->ShowQuests();
	}
}

void AZPlayerController::ToggleSkills()
{
	if (HUDWidget)
	{
		HUDWidget->ShowSkills();
	}
}

void AZPlayerController::ToggleMap()
{
	if (HUDWidget)
	{
		HUDWidget->ShowMap();
	}
}

void AZPlayerController::StartLockpicking(AZLockableBase* Target)
{
	if (!ensure(IsLocalPlayerController()))
	{
		return;
	}
	if (!ensure(Target))
	{
		return;
	}
	if (InteractableTarget)
	{
		ensure(0);
		return;
	}

	InteractableTarget = Target;
	PlayerActivity = EZPlayerActivity::Lockpicking;

	Lock->SetActorTransform(Target->GetLockTransform());
	Lock->StartLockpicking();
	SetViewTargetWithBlend(Lock, LockBlendTime, EViewTargetBlendFunction::VTBlend_Cubic);

	if (HUDWidget)
	{
		HUDWidget->StartLockpicking(Lock);
	}

	SwitchMappingContexts(EZInputMode::Lockpicking);
}

void AZPlayerController::StopCurrentInteraction()
{
	FPCharacter->StopInteractingWith(InteractableTarget);
}

void AZPlayerController::StopLockpicking()
{
	if (!InteractableTarget)
	{
		return;
	}
	if (!ensure(IsLocalPlayerController()))
	{
		return;
	}
	SetViewTargetWithBlend(GetPawn(), 0.5f, VTBlend_Cubic);
	Lock->StopLockpicking();

	if (HUDWidget)
	{
		HUDWidget->StopLockpicking();
	}

	SwitchMappingContexts(EZInputMode::Default);
	InteractableTarget = nullptr;
	PlayerActivity = EZPlayerActivity::None;
}

void AZPlayerController::Escape()
{
	if (HUDWidget)
	{
		HUDWidget->ShowPauseWidget();
		AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
		if (GS->PlayerArray.Num() <= 1)
		{
			SetPause(true);
		}
	}
}

void AZPlayerController::Unpause()
{
	SetPause(false);
}

void AZPlayerController::AddNotification(EZNotificationType NotificationType, const FText& Message)
{
	if (AZHUD* HUD = GetHUD<AZHUD>())
	{
		HUD->AddNotification(NotificationType, Message);
	}
}

AZPlayerCameraManager* AZPlayerController::GetPlayerCameraManager() const
{
	return Cast<AZPlayerCameraManager>(PlayerCameraManager);
}

ETeamAttitude::Type AZPlayerController::GetTeamAttitudeTowards(const AActor& Other) const
{
	return IGenericTeamAgentInterface::GetTeamAttitudeTowards(Other);
}

void AZPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	UE_LOG(LogTemp, Warning, TEXT("AcknowledgePossession"));
	if (IsLocalPlayerController())
	{
		if (FPCharacter)
		{
			LocalDeinit();
		}
		FPCharacter = Cast<AZFPCharacter>(P);
		if (FPCharacter)
		{
			LocalInit();
		}
	}
}

void AZPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Warning, TEXT("OnPossess"));
	FPCharacter = Cast<AZFPCharacter>(InPawn);
}

void AZPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetDefaultFOV(80.f);

	if (IsLocalPlayerController())
	{
		if (!ensure(LockClass))
		{
			return;
		}
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		Lock = GetWorld()->SpawnActor<AZLock>(LockClass, SpawnParams);
	}
}

void AZPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (IsLocalPlayerController())
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EnhancedInputComponent->BindAction(LockpickRotateAction, ETriggerEvent::Triggered, this, &ThisClass::LockpickRotate);
		
			EnhancedInputComponent->BindAction(LockpickTensionAction, ETriggerEvent::Started, this, &ThisClass::LockpickTensionStart);
			EnhancedInputComponent->BindAction(LockpickTensionAction, ETriggerEvent::Completed, this, &ThisClass::LockpickTensionEnd);

			EnhancedInputComponent->BindAction(LockpickCancelAction, ETriggerEvent::Started, this, &ThisClass::LockpickCancel);
		}

		SwitchMappingContexts(EZInputMode::Default);
	}
}

void AZPlayerController::OnCharacterActivityStarted(const FZInteractionState& InteractionState)
{
	if (!IsLocalPlayerController())
	{
		ensure(0);
		return;
	}
	switch (InteractionState.Activity)
	{
		case EZCharacterActivity::None:
			break;
		case EZCharacterActivity::Sleeping:
			break;
		case EZCharacterActivity::Sitting:
			break;
		case EZCharacterActivity::Looting:
			if (ensure(InteractionState.Params.IsType<FZLootingParams>()))
			{
				StartLooting(InteractionState.InteractionTargetActor, InteractionState.Params);
			}
			break;
		case EZCharacterActivity::PickingUp:
			break;
		case EZCharacterActivity::Lockpicking:
			StartLockpicking(Cast<AZLockableBase>(InteractionState.InteractionTargetActor));
			break;
		case EZCharacterActivity::Talking:
		{
			AZNPCharacter* NPC = Cast<AZNPCharacter>(InteractionState.InteractionTargetActor);
			if (InteractionState.Params.IsType<FZNPCDialogueParams>())
			{
				StartNPCInitiatedDialogue(NPC, InteractionState.Params);
			}
			else if (InteractionState.Params.IsType<FZDialogueParams>())
			{
				StartDialogue(NPC, InteractionState.Params);
			}
			else
			{
				ensure(0);
			}
			break;
		}
		case EZCharacterActivity::Smoking:
			break;
		case EZCharacterActivity::Surrendering:
			break;
		default:
			ensure(0);
			break;
	}
}

void AZPlayerController::OnCharacterActivityUpdated(const FZInteractionState& InteractionState)
{
	if (!IsLocalPlayerController())
	{
		ensure(0);
		return;
	}
	switch (InteractionState.Activity)
	{
		case EZCharacterActivity::None:
			break;
		case EZCharacterActivity::Sleeping:
			break;
		case EZCharacterActivity::Sitting:
			break;
		case EZCharacterActivity::Looting:
			break;
		case EZCharacterActivity::PickingUp:
			break;
		case EZCharacterActivity::Lockpicking:
			break;
		case EZCharacterActivity::Talking:
			if (ensure(InteractionState.Params.IsType<FZDialogueParams>()))
			{
				const FZDialogueParams& DialogueParams = InteractionState.Params.Get<FZDialogueParams>();
				HUDWidget->PushDialogueOptions(Cast<AZNPCharacter>(InteractionState.InteractionTargetActor), DialogueParams);
			}
			break;
		case EZCharacterActivity::Smoking:
			break;
		case EZCharacterActivity::Surrendering:
			break;
		default:
			ensure(0);
			break;
	}
}

void AZPlayerController::OnCharacterActivityStopped(EZCharacterActivity Activity)
{
	if (!IsLocalPlayerController())
	{
		ensure(0);
		return;
	}
	switch (Activity)
	{
		case EZCharacterActivity::None:
			break;
		case EZCharacterActivity::Sleeping:
			break;
		case EZCharacterActivity::Sitting:
			break;
		case EZCharacterActivity::Looting:
			StopLooting();
			break;
		case EZCharacterActivity::PickingUp:
			break;
		case EZCharacterActivity::Lockpicking:
			StopLockpicking();
			break;
		case EZCharacterActivity::Talking:
			EndDialogue();
			break;
		case EZCharacterActivity::Smoking:
			break;
		case EZCharacterActivity::Surrendering:
			break;
		default:
			ensure(0);
			break;
	}
}

void AZPlayerController::SetDefaultFOV(float FOV)
{
	if (AZPlayerCameraManager* PCM = Cast<AZPlayerCameraManager>(PlayerCameraManager))
	{
		PCM->SetDefaultFOV(FOV);
		PCM->SetFOV(FOV);
	}
}

void AZPlayerController::OnCharacterDeath()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
	}
	FadeToBlack(DeathFadeDuration);
}

void AZPlayerController::FadeToBlack(float Duration)
{
    PlayerCameraManager->StartCameraFade(
        0.f,                    // From alpha
        1.f,                    // To alpha (1 = fully black)
        Duration,               // Time
        FLinearColor::Black,    // Color
        false,                  // Fade audio
        true                    // Hold when finished
    );
}

void AZPlayerController::LocalInit()
{
	LocalDeinit();

	if (!ensure(IsLocalPlayerController()))
	{
		return;
	}
	InitHUD();

	BIND_MULTICAST_UOBJECT(OnCharacterDeathConnection, FPCharacter, FPCharacter->OnDeath, this, &ThisClass::OnCharacterDeath);
	BIND_MULTICAST_UOBJECT(OnInteractionActivityStartedConnection, FPCharacter, FPCharacter->OnInteractionActivityStarted, this, &ThisClass::OnCharacterActivityStarted);
	BIND_MULTICAST_UOBJECT(OnInteractionActivityUpdatedConnection, FPCharacter, FPCharacter->OnInteractionActivityUpdated, this, &ThisClass::OnCharacterActivityUpdated);
	BIND_MULTICAST_UOBJECT(OnActivityStoppedConnection, FPCharacter, FPCharacter->OnActivityStopped, this, &ThisClass::OnCharacterActivityStopped);
}

void AZPlayerController::LocalDeinit()
{
	HUDWidget = nullptr;

	OnCharacterDeathConnection.Disconnect();
	OnInteractionActivityStartedConnection.Disconnect();
	OnInteractionActivityUpdatedConnection.Disconnect();
	OnActivityStoppedConnection.Disconnect();
}

void AZPlayerController::InitHUD()
{
	if (AZHUD* HUD = GetHUD<AZHUD>())
	{
		HUD->Init(FPCharacter);
	}
}

void AZPlayerController::CompleteDialogueEvent(FGameplayTag DialogueID, AZNPCharacter* NPC)
{
	ensure(DialogueID.IsValid() && NPC);
	if (!HasAuthority())
	{
		Server_Reliable_CompleteDialogueEvent(DialogueID, NPC);
		return;
	}
	AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
	if (!ensure(GS))
	{
		return;
	}

	const FZDialogueOptionRow* CompletedDialogueRow = GS->GetDialogueOption(DialogueID);
	if (ensure(CompletedDialogueRow) && CompletedDialogueRow->bLocksDialogue)
	{
		NPC->SetLockedDialogueID(CompletedDialogueRow->ID);
	}
	else
	{
		NPC->SetLockedDialogueID(FGameplayTag::EmptyTag);
	}
	GS->CompleteEvent(DialogueID, NPC, this);
	if (CompletedDialogueRow && CompletedDialogueRow->bEndsDialogue)
	{
		return; // Don't update dialogue options if dialogue ended
	}
	const FZInteractionParams NewParams = NPC->GetInteractionParams(FPCharacter);
	ensure(NPC->GetCharacterActivity() == EZCharacterActivity::Talking);
	FPCharacter->UpdateActivity(FZInteractionState(NPC, NPC->GetCharacterActivity(), NewParams));
}

void AZPlayerController::Server_Reliable_CompleteDialogueEvent_Implementation(FGameplayTag DialogueID, AZNPCharacter* NPC)
{
	CompleteDialogueEvent(DialogueID, NPC);
}

void AZPlayerController::Server_Reliable_UnlockLockpickTarget_Implementation(AZLockableBase* LockpickTarget)
{
	if (!ensure(LockpickTarget == FPCharacter->GetInteractionTarget(EZCharacterActivity::Lockpicking).GetObject()))
	{
		return;
	}
	LockpickTarget->Unlock();
}

void AZPlayerController::SaveGame(const FString& SlotName)
{
	if (!HasAuthority())
	{
		return;
	}
	UZSaveSubsystem* SaveSubSystem = GetGameInstance()->GetSubsystem<UZSaveSubsystem>();
	SaveSubSystem->SaveGame(SlotName);
}

void AZPlayerController::LoadGame(const FString& SlotName)
{
	if (!HasAuthority())
	{
		return;
	}
	UZSaveSubsystem* SaveSubSystem = GetGameInstance()->GetSubsystem<UZSaveSubsystem>();
	SaveSubSystem->LoadGame(SlotName);
}

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT

void AZPlayerController::Slomo(float Value)
{
	UGameplayStatics::SetGlobalTimeDilation(this, Value);
}

void AZPlayerController::SetTimeOfDay(float Time)
{
	if (AZGameState* GS = GetWorld()->GetGameState<AZGameState>())
	{ 
		GS->SetTimeOfDay(Time);
	}
}

void AZPlayerController::FreeXP(int32 XP)
{
	if (AZPlayerState* PS = GetPlayerState<AZPlayerState>())
	{
		PS->GetLevelingComponent()->AddXP(XP);
	}
}

void AZPlayerController::SetVolume(float Volume)
{
	if (FAudioDevice* AudioDevice = GEngine->GetMainAudioDeviceRaw())
    {
        AudioDevice->SetTransientPrimaryVolume(Volume);
    }
}

void AZPlayerController::ChangeReputation(int32 Faction, float ReputationChange)
{
	if (AZGameState* GS = GetWorld()->GetGameState<AZGameState>())
	{
		if (UEnum* EnumClass = StaticEnum<EZFaction>())
		{
			if (EnumClass->IsValidEnumValue(Faction))
			{
				const EZFaction ValidFaction = static_cast<EZFaction>(Faction);
				GS->ChangeReputation(static_cast<EZFaction>(ValidFaction), ReputationChange);
			}
			else
			{
				UE_LOG(LogZombies, Warning, TEXT("Invalid faction: %d"), Faction);
			}
		}
	}
}

#endif

void AZPlayerController::LockpickRotate(const FInputActionValue& Value)
{
	if (Lock && IsLockpicking())
	{
		Lock->RotateLockpick(Value.Get<float>());
	}
}

void AZPlayerController::LockpickTensionStart()
{
	if (Lock && IsLockpicking())
	{
		Lock->TensionStart();
	}
}

void AZPlayerController::LockpickTensionEnd()
{
	if (Lock)
	{
		Lock->TensionEnd();
	}
}

void AZPlayerController::LockpickCancel()
{
	StopCurrentInteraction();
	HUDWidget->StopLockpicking();
}

void AZPlayerController::OnLockpickingFinished(bool bSuccess)
{
	if (bSuccess)
	{
		AZLockableBase* LockpickTarget = Cast<AZLockableBase>(InteractableTarget.GetObject());
		if (ensure(LockpickTarget))
		{
			if (HasAuthority())
			{
				LockpickTarget->Unlock();
			}
			else
			{
				Server_Reliable_UnlockLockpickTarget(LockpickTarget);
			}
		}
	}
	else
	{
		StopCurrentInteraction();
	}
	if (!HasAuthority())
	{
		StopLockpicking(); // Make client feel responsive
	}
}

void AZPlayerController::OnLootingFinished()
{
	StopCurrentInteraction();
}

void AZPlayerController::OnDialogueFinished()
{
	if (HUDWidget)
	{
		HUDWidget->EndDialogue(); // Dialogue widget does not close itself
	}
	StopCurrentInteraction();
}

void AZPlayerController::InitPlayerState()
{
	if (AZPlayerState* OldPS = GetPlayerState<AZPlayerState>())
	{
		CleanupZPlayerState(OldPS);
	}
	Super::InitPlayerState();

	InitZPlayerState(GetPlayerState<AZPlayerState>());
}

void AZPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitZPlayerState(GetPlayerState<AZPlayerState>());
}

void AZPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (AZPlayerState* PS = GetPlayerState<AZPlayerState>())
    {
        CleanupZPlayerState(PS);
    }
}

void AZPlayerController::SwitchMappingContexts(EZInputMode NewInputMode)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (FZMappingContexts* CurrentContexts = MappingContexts.Find(CurrentInputMode))
		{
			for (const FZMappingContextEntry& MappingContext : CurrentContexts->MappingContexts)
			{
				Subsystem->RemoveMappingContext(MappingContext.MappingContext);
			}
		}
		if (FZMappingContexts* NewContexts = MappingContexts.Find(NewInputMode))
		{
			for (const FZMappingContextEntry& MappingContext : NewContexts->MappingContexts)
			{
				Subsystem->AddMappingContext(MappingContext.MappingContext, MappingContext.Priority);
			}
		}
		CurrentInputMode = NewInputMode;
	}
}

void AZPlayerController::InitZPlayerState(AZPlayerState* PS)
{
	if (!IsLocalPlayerController() || !PS)
	{
		return;
	}

	UZLevelingComponent* LevelingComponent =  PS->GetLevelingComponent();
	BIND_MULTICAST_UOBJECT(OnXPGainedConnection, LevelingComponent, LevelingComponent->OnXPGained, this, &ThisClass::OnXPGained);
	BIND_MULTICAST_UOBJECT(OnLevelUpConnection, LevelingComponent, LevelingComponent->OnLevelUp, this, &ThisClass::OnLevelUp);
	UZSkillsComponent* SkillsComponent = PS->GetSkillsComponent();
	BIND_MULTICAST_UOBJECT(OnLevelUpConnection, SkillsComponent, SkillsComponent->OnSkillUnlocked, this, &ThisClass::OnSkillUnlocked);
}

void AZPlayerController::CleanupZPlayerState(AZPlayerState* PS)
{
	if (!IsLocalPlayerController() || !PS)
    {
        return;
    }
	OnXPGainedConnection.Disconnect();
	OnLevelUpConnection.Disconnect();
	OnSkillUnlockedConnection.Disconnect();
}

void AZPlayerController::OnXPGained(int32 XP)
{
	AddNotification(EZNotificationType::XPEarned, FText::Format(ZText::XPEarned, XP));
}

void AZPlayerController::OnLevelUp(int32 Level)
{
	AddNotification(EZNotificationType::LeveledUp, LOCTEXT("LeveledUp", "Leveled up"));
}

void AZPlayerController::OnSkillUnlocked(EZSkill Skill)
{
	if (const UZGameInstance* GI = GetGameInstance<UZGameInstance>())
	{
		if (const FZSkillData* SkillData = GI->GetSkillData(Skill))
		{
			AddNotification(EZNotificationType::SkillUnlocked, FText::Format(ZText::SkillUnlocked, SkillData->Name));
		}
		else
		{
			UE_LOG(LogZombies, Error, TEXT("Failed to find skill data for skill %s"), *ENUM_TO_STRING(EZSkill, Skill));
		}
	}
}

#undef LOCTEXT_NAMESPACE

