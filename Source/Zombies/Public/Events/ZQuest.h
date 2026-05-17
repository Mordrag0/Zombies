// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "ZQuest.generated.h"

enum class EZFaction : uint8;

USTRUCT(BlueprintType)
struct FZQuestObjective
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag RequiredEvent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText Description;
};

USTRUCT(BlueprintType)
struct FZQuestRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FZQuestRow() : XPReward(0) {}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag RequiredEvent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer FailEvents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 XPReward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EZFaction, float> ReputationChanges;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FZQuestObjective> Objectives;

	bool RequiresObjective(FGameplayTag Event) const;

	bool AreAllObjectivesCompleted(const TSet<FGameplayTag>& CompletedEvents) const;
};

