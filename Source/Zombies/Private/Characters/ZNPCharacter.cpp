// Copyright 2026 Luka Markuš. All rights reserved.


#include "Characters/ZNPCharacter.h"
#include "ZPlayerController.h"
#include "Weapons/ZWeapon.h"
#include "Engine/AssetManager.h"
#include "Inventory/ZInventoryComponent.h"
#include "Inventory/ZInventoryItemData.h"
#include "Weapons/ZRangedWeapon.h"
#include "Net/UnrealNetwork.h"
#include "ZGameState.h"
#include "AI/ZNPCAIController.h"
#include "ZTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIController.h"
#include "AI/ZAIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Movement/ZCharacterMoverComponent.h"
#include "MoverComponent.h"
#include "Movement/ZNavMoverComponent.h"


AZNPCharacter::AZNPCharacter()
{
	bHasInfiniteAmmo = true;
	bCanDie = true;
	Faction = EZFaction::NoFaction;

	SleepStartTime = 20;
	SleepEndTime = 6;
}

void AZNPCharacter::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult)
{
	Super::ProduceInput_Implementation(SimTimeMs, InputCmdResult);
	
	FCharacterDefaultInputs& CharacterInputs = InputCmdResult.InputCollection.FindOrAddMutableDataByType<FCharacterDefaultInputs>();
	const AZNPCAIController* NPCAIController = GetController<AZNPCAIController>();
	if (!bMovementEnabled || !NPCAIController)
	{
		static const FCharacterDefaultInputs DoNothingInput;
		CharacterInputs = DoNothingInput;
		return;
	}
	
	CharacterInputs.ControlRotation = NPCAIController->GetControlRotation();
	
	FVector MoveInputIntent = FVector::ZeroVector;
	FVector MoveInputVelocity = FVector::ZeroVector;
	const bool bRequestedNavMovement = GetNavMoverComponent()->ConsumeNavMovementData(MoveInputIntent, MoveInputVelocity);
	
	// Favor velocity input 
	const bool bUsingInputIntentForMove = MoveInputVelocity.IsZero();
	if (bUsingInputIntentForMove)
	{
		const FVector FinalDirectionalIntent = CharacterInputs.ControlRotation.RotateVector(MoveInputIntent);
		CharacterInputs.SetMoveInput(EMoveInputType::DirectionalIntent, FinalDirectionalIntent);
	}
	else
	{
		CharacterInputs.SetMoveInput(EMoveInputType::Velocity, MoveInputVelocity);
	}
	
	static float RotationMagMin(1e-3);
	const bool bHasAffirmativeMoveInput = (CharacterInputs.GetMoveInput().Size() >= RotationMagMin);
	
	// Figure out intended orientation
	if (bHasAffirmativeMoveInput && CharacterInputs.OrientationIntent.IsZero())
	{
		if (bOrientRotationToMovement)
		{
			// set the intent to the actors movement direction
			CharacterInputs.OrientationIntent = CharacterInputs.GetMoveInput().GetSafeNormal();
		}
		else
		{
			// set intent to the the control rotation - often a player's camera rotation
			CharacterInputs.OrientationIntent = CharacterInputs.ControlRotation.Vector().GetSafeNormal();
		}
	}
	
	CharacterInputs.bIsJumpPressed = bIsJumpPressed;
	CharacterInputs.bIsJumpJustPressed = bIsJumpJustPressed;
	CharacterInputs.SuggestedMovementMode = NAME_None;

	// Convert inputs to be relative to the current movement base (depending on options and state)
	CharacterInputs.bUsingMovementBase = false;

	if (bUseBaseRelativeMovement)
	{
		if (const UZCharacterMoverComponent* MoverComp = GetMoverComponent())
		{
			if (UPrimitiveComponent* MovementBase = MoverComp->GetMovementBase())
			{
				const FName MovementBaseBoneName = MoverComp->GetMovementBaseBoneName();

				FVector RelativeMoveInput, RelativeOrientDir;

				UBasedMovementUtils::TransformWorldDirectionToBased(MovementBase, MovementBaseBoneName, CharacterInputs.GetMoveInput(), RelativeMoveInput);
				UBasedMovementUtils::TransformWorldDirectionToBased(MovementBase, MovementBaseBoneName, CharacterInputs.OrientationIntent, RelativeOrientDir);

				CharacterInputs.SetMoveInput(CharacterInputs.GetMoveInputType(), RelativeMoveInput);
				CharacterInputs.OrientationIntent = RelativeOrientDir;

				CharacterInputs.bUsingMovementBase = true;
				CharacterInputs.MovementBase = MovementBase;
				CharacterInputs.MovementBaseBoneName = MovementBaseBoneName;
			}
		}
	}

	// Clear/consume temporal movement inputs. We are not consuming others in the event that the game world is ticking at a lower rate than the Mover simulation. 
	// In that case, we want most input to carry over between simulation frames.
	{
		bIsJumpJustPressed = false;
	}
}

const UZInventoryItemData* AZNPCharacter::PickBestWeapon() const
{
	const FZReplicatedInventory& Inventory = GetInventoryComponent()->GetInventory();
	bool bBestIsRanged = false;
	const UZInventoryItemData* BestItemData = nullptr;
	for (const FZInventoryEntry& Item : Inventory.Items)
	{
		const UZInventoryItemData* ItemData = Item.ItemData;
		if (!ItemData)
		{
			continue;
		}
		if (ItemData->ItemClass->IsChildOf(AZRangedWeapon::StaticClass()))
		{
			if (!BestItemData || (ItemData->Value > BestItemData->Value)) // #ZTODO check if has ammo over [minimum ammo to equip]
			{
				BestItemData = ItemData;
				bBestIsRanged = true;
			}
		}
		else if (ItemData->ItemClass->IsChildOf(AZWeapon::StaticClass()))
		{
			if (!BestItemData || (!bBestIsRanged && (ItemData->Value > BestItemData->Value)))
			{
				BestItemData = ItemData;
			}
		}
	}
	
    return BestItemData;
}

void AZNPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

}

void AZNPCharacter::SetLockedDialogueID(FGameplayTag DialogueId)
{
	LockedDialogueID = DialogueId;
}

bool AZNPCharacter::IsPendingDialogue(AZPlayerController* Player) const
{
	return GetFirstPendingDialogue(Player).DialogueID.IsValid();
}

bool AZNPCharacter::IsPendingDialogue(AZPlayerController* Player, FGameplayTag DialogueID) const
{
	if (const FZPendingDialogueList* PlayerPendingDialogues = PendingDialogues.Find(Player))
	{
		return PlayerPendingDialogues->Dialogue.ContainsByPredicate([&DialogueID](const FZPendingDialogue& PendingDialogue)
		{
			return PendingDialogue.DialogueID == DialogueID;
		});
	}
	return false;
}

void AZNPCharacter::AddPendingDialogue(AZPlayerController* Player, FGameplayTag DialogueID, EZDialogueContext InDialogueContext)
{
	FZPendingDialogueList& PlayerPendingDialogues = PendingDialogues.FindOrAdd(Player);
    
    // Insert sorted by context priority (higher context value = higher priority)
    int32 InsertIndex = PlayerPendingDialogues.Dialogue.Num();
    for (int32 Idx = 0; Idx < PlayerPendingDialogues.Dialogue.Num(); Idx++)
    {
        if (InDialogueContext > PlayerPendingDialogues.Dialogue[Idx].DialogueContext)
        {
            InsertIndex = Idx;
            break;
        }
    }
    PlayerPendingDialogues.Dialogue.Insert(FZPendingDialogue(DialogueID, InDialogueContext), InsertIndex);
}

FZPendingDialogue AZNPCharacter::GetFirstPendingDialogue(AZPlayerController* Player) const
{
	const FZPendingDialogueList* PlayerPendingDialogues = PendingDialogues.Find(Player);
	if (PlayerPendingDialogues && PlayerPendingDialogues->Dialogue.Num() > 0)
	{
		return PlayerPendingDialogues->Dialogue[0];
	}
	return FZPendingDialogue();
}

void AZNPCharacter::RemovePendingDialogue(AZPlayerController* Player, FGameplayTag DialogueID)
{
	FZPendingDialogueList* PlayerPendingDialogues = PendingDialogues.Find(Player);
	if (PlayerPendingDialogues)
	{
		PlayerPendingDialogues->Dialogue.RemoveAll([&DialogueID](const FZPendingDialogue& PendingDialogue)
		{
			return PendingDialogue.DialogueID == DialogueID;
		});
		if (PlayerPendingDialogues->Dialogue.Num() == 0)
		{
			PendingDialogues.Remove(Player);
		}
	}
}

void AZNPCharacter::SetDialogueContext(EZDialogueContext InContext)
{
	DialogueContext = InContext;
}

EZDialogueContext AZNPCharacter::GetDialogueContext() const
{
	return DialogueContext;
}

void AZNPCharacter::ClearDialogueContext()
{
	DialogueContext = EZDialogueContext::None;
}

void AZNPCharacter::CompleteContextDialogue(FGameplayTag EventID)
{
	CompletedContextDialogues.AddTag(EventID);
}

FGameplayTag AZNPCharacter::GetDialogueContextInitiationID(EZDialogueContext Context) const
{
	if (const FGameplayTag* DialogueID = DialogueContextInitiationIDs.Find(Context))
	{
		return *DialogueID;
	}
	return FGameplayTag::EmptyTag;
}

void AZNPCharacter::Confront(AZPlayerController* Player, EZDialogueContext InDialogueContext)
{
	const FGameplayTag DialogueID = GetDialogueContextInitiationID(InDialogueContext);
	if (DialogueID.IsValid())
	{
		AddPendingDialogue(Player, DialogueID, InDialogueContext);
	}
}

void AZNPCharacter::GetAimViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	GetActorEyesViewPoint(OutLocation, OutRotation);
	if (AZAIController* AIController = GetController<AZAIController>())
	{
		if (AZCharacter* CombatTargetCharacter = AIController->GetCombatTarget())
		{
			const FVector TargetLocation = CombatTargetCharacter->GetMesh()->GetSocketLocation(FName("spine_03")); // #ZTODO: some NPCs aim for head also make sure the character is rotated this way?
			OutRotation = (TargetLocation - OutLocation).Rotation();
		}
	}
}

void AZNPCharacter::StartInteractionActivity(const FZInteractionState& InInteractionState)
{
	// Super is at the end so we can set dialogue state (context and locked id) before OnActivityStarted broadcasts
	switch (InInteractionState.Activity)
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
		{
			AZCharacter* InteractingFPCharacter = Cast<AZCharacter>(InInteractionState.InteractionTargetActor);
			AZPlayerController* PC = InteractingFPCharacter ? InteractingFPCharacter->GetController<AZPlayerController>() : nullptr;
			FZPendingDialogue PendingDialogue = GetFirstPendingDialogue(PC);
			if (PendingDialogue.DialogueID.IsValid())
			{
				SetDialogueContext(PendingDialogue.DialogueContext);
				AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
				const FZDialogueOptionRow* DialogueOptionRow = GS->GetDialogueOption(PendingDialogue.DialogueID);
				if (ensure(DialogueOptionRow) && DialogueOptionRow->bLocksDialogue)
				{
					SetLockedDialogueID(PendingDialogue.DialogueID);
				}
			}
			break;
		}
		case EZCharacterActivity::Smoking:
			break;
		case EZCharacterActivity::Surrendering:
			break;
		case EZCharacterActivity::MAX:
			break;
		default:
			break;
	}
	Super::StartInteractionActivity(InInteractionState);
}

void AZNPCharacter::StopActivity(EZCharacterActivity Activity)
{
	switch (Activity)
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
			ClearDialogueContext();
			break;
		case EZCharacterActivity::Smoking:
			break;
		case EZCharacterActivity::Surrendering:
			break;
		case EZCharacterActivity::MAX:
			break;
		default:
			ensure(0);
			break;
	}
	Super::StopActivity(Activity);
}

TScriptInterface<IZInteractable> AZNPCharacter::GetAssignedInteractable(EZCharacterActivity Activity)
{
	return TScriptInterface<IZInteractable>(AssignedInteractables.FindRef(Activity));
}

bool AZNPCharacter::CanInteract(const AZCharacter* InCharacter) const
{
	if (AZNPCAIController* AIController = GetController<AZNPCAIController>())
	{
		if (AIController->GetCombatTarget())
		{
			return false;
		}
	}
	return Super::CanInteract(InCharacter);
}

EZCharacterActivity AZNPCharacter::GetCharacterActivity() const
{
	return bDead ? EZCharacterActivity::Looting : EZCharacterActivity::Talking;
}

FZInteractionParams AZNPCharacter::GetInteractionParams(const AZCharacter* InCharacter) const
{
	if (GetCharacterActivity() == EZCharacterActivity::Talking)
	{
		const FZPendingDialogue PendingDialogue = GetFirstPendingDialogue(InCharacter->GetController<AZPlayerController>());
		if (PendingDialogue.DialogueID.IsValid())
		{
			return FZInteractionParams(TInPlaceType<FZNPCDialogueParams>{}, PendingDialogue.DialogueID, PendingDialogue.DialogueContext);
		}
		else
		{
			AZPlayerController* PC = InCharacter->GetController<AZPlayerController>();
			AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
			TArray<FGameplayTag> IDs = GS->GetDialogueOptionIDs(PC, this);
			return FZInteractionParams(TInPlaceType<FZDialogueParams>{}, GetLockedDialogueID().IsValid(), GetDialogueContext(), MoveTemp(IDs));
		}
	}
	return Super::GetInteractionParams(InCharacter);
}

void AZNPCharacter::OnInteractionAttempted(AZCharacter* InCharacter)
{
	if (bDead)
	{
		return;
	}
	if (IsPerformingActivity(EZCharacterActivity::Sleeping))
	{
		if (ensure(InteractionTargets.Contains(EZCharacterActivity::Sleeping)))
		{
			StopInteractingWith(InteractionTargets[EZCharacterActivity::Sleeping]); // Wake up
		}
		// #ZTODO AddPendingDialogue(InCharacter->GetController<AZPlayerController>(), GetDialogueContextInitiationID(EZDialogueContext::Sleeping), EZDialogueContext::Sleeping);
	}
}

void AZNPCharacter::BeginPlay()
{
	Super::BeginPlay();

	ensure(Name.IsValid());
}

void AZNPCharacter::SetCanTrade(bool bInCanTrade)
{
	bCanTrade = bInCanTrade;
}

void AZNPCharacter::Die(AController* DeathInstigator)
{
	if (bCanDie)
	{
		Super::Die(DeathInstigator);
	}
}

UAIPerceptionComponent* AZNPCharacter::GetAIPerceptionComponent() const
{
	AAIController* AIController = GetController<AAIController>();
	if (!AIController)
	{
		return nullptr;
	}
	return AIController->GetAIPerceptionComponent();
}

void AZNPCharacter::SetSleeping(bool bSleeping)
{
	Super::SetSleeping(bSleeping);

	if (UAIPerceptionComponent* Perception = GetAIPerceptionComponent())
	{
		Perception->SetSenseEnabled(UAISense_Sight::StaticClass(), !bSleeping);
	}
}

