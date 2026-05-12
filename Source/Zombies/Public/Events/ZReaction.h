// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "ZReaction.generated.h"

UENUM()
enum class EZReactionType : uint8
{
	None,
	Attack,
	TakeBackStolenItems,
	WarnAboutBreakingIn,
};

USTRUCT()
struct FZReactionRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EventID;

	//UPROPERTY(EditDefaultsOnly)
	//FGameplayTag NPC;
};

USTRUCT()
struct FZSimpleReactionRow : public FZReactionRow
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EZReactionType ReactionType = EZReactionType::None;
};

USTRUCT()
struct FZPathReactionRow : public FZReactionRow
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PathTag;

	UPROPERTY(EditDefaultsOnly)
	bool bEscortPlayer = false;
};

USTRUCT()
struct FZHomeTransformReactionRow : public FZReactionRow
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WaypointTag;
};

USTRUCT()
struct FZTakeBackStolenItemsReaction : public FZReactionRow
{
	GENERATED_BODY()
};
