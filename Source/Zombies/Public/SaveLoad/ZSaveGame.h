// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ZGameState.h"
#include "GameplayTagContainer.h"
#include "ZSaveGame.generated.h"

struct FZSaveWorldVersion
{
    static const FGuid GUID;

    enum Type
    {
        Initial = 0,
        //AddedStamina,
        //SplitHealth,
        //RemovedInventory,

        VersionPlusOne,
        LatestVersion = VersionPlusOne - 1
    };
};

USTRUCT()
struct FZActorSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FGuid ActorGUID;

	UPROPERTY(SaveGame)
	FString Name;

	UPROPERTY(SaveGame)
	FTransform Transform;

	UPROPERTY(SaveGame)
	TArray<uint8> ByteData;
};

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(SaveGame)
	TArray<FZActorSaveData> SavedActors;

	UPROPERTY(SaveGame)
	int32 SaveVersion = FZSaveWorldVersion::Type::LatestVersion;

	// Game state
	UPROPERTY(SaveGame)
	FGameplayTagContainer CompletedEvents;

	UPROPERTY(SaveGame)
	FGameplayTagContainer AvailableDialogueEvents;

	UPROPERTY(SaveGame)
	FGameplayTagContainer AvailablePathEvents;

	UPROPERTY(SaveGame)
	FGameplayTagContainer CompletedQuests;

	UPROPERTY(SaveGame)
	FGameplayTagContainer OpenedQuests;

	UPROPERTY(SaveGame)
	FGameplayTagContainer FailedQuests;

	UPROPERTY(SaveGame)
	FGameplayTagContainer UnavailableQuests;

	UPROPERTY(SaveGame)
	TArray<FZFactionReputation> FactionReputation;
};

