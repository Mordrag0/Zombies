// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ZNetworkTypes.generated.h"

class UZInventoryComponent;
class IZInteractable;
enum class EZDialogueContext : uint8;
enum class EZCharacterActivity : uint32;

struct FZNoInteractionParams {};
struct FZLootingParams
{
	FZLootingParams() = default;
	FZLootingParams(UZInventoryComponent* InInventoryComponent) : InventoryComponent(InInventoryComponent) {}

    UZInventoryComponent* InventoryComponent = nullptr;
};
struct FZNPCDialogueParams
{
	FZNPCDialogueParams() = default;
	FZNPCDialogueParams(FGameplayTag InPendingDialogueID, EZDialogueContext InDialogueContext)
		: PendingDialogueID(InPendingDialogueID), DialogueContext(InDialogueContext) {}

	FGameplayTag PendingDialogueID;
	EZDialogueContext DialogueContext;
};
struct FZDialogueParams
{
	FZDialogueParams() = default;
	FZDialogueParams(bool bInDialogueLocked, EZDialogueContext InDialogueContext, const TArray<FGameplayTag>& InDialogueIDs) 
		: bDialogueLocked(bInDialogueLocked), DialogueContext(InDialogueContext), DialogueIDs(InDialogueIDs) {}

	bool bDialogueLocked;
	EZDialogueContext DialogueContext;
	TArray<FGameplayTag> DialogueIDs;
};
using FZInteractionParams = TVariant<FZNoInteractionParams, FZLootingParams, FZNPCDialogueParams, FZDialogueParams>;


USTRUCT()
struct FZInteractionState
{
    GENERATED_BODY()

	FZInteractionState() = default;
	FZInteractionState(TScriptInterface<IZInteractable> InTarget, EZCharacterActivity InActivity, const FZInteractionParams& InParams) 
		: InteractionTargetActor(Cast<AActor>(InTarget.GetObject())), Activity(InActivity), Params(InParams) {}

    UPROPERTY()
	TObjectPtr<AActor> InteractionTargetActor;

    EZCharacterActivity Activity;

	FZInteractionParams Params = FZInteractionParams(TInPlaceType<FZNoInteractionParams>{}); // TVariant, not UPROPERTY

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess); // #ZTODO Iris replication fragments

	bool operator==(const FZInteractionState& Other) const
	{
		return false; // Always consider states different to force replication // #ZTODO why not compare all the properties including Params?
	}
};

template<>
struct TStructOpsTypeTraits<FZInteractionState> : public TStructOpsTypeTraitsBase2<FZInteractionState>
{
    enum { 
		WithNetSerializer = true,
		WithIdenticalViaEquality = true,
	};
};

