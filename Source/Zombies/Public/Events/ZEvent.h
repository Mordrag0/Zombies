// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "ZEvent.generated.h"

enum class EZSkill : uint8;
class AZPlayerController;
class AZNPCAIController;
enum class EZDialogueContext : uint8;
enum class EZFaction : uint8;
struct FZEventRow;

USTRUCT()
struct ZOMBIES_API FZEventCondition
{
	GENERATED_BODY()
	
	virtual ~FZEventCondition() = default;
	bool operator==(const FZEventCondition& Other) const = default;
	bool operator!=(const FZEventCondition& Other) const = default;

	virtual bool IsMet(const AZPlayerController* PC) const { unimplemented(); return false; }
	virtual FString ToString() const {unimplemented(); return TEXT("Invalid");}
};

USTRUCT()
struct ZOMBIES_API FZEventCondition_Reputation : public FZEventCondition
{
	GENERATED_BODY()
	
	virtual ~FZEventCondition_Reputation() override = default;
	bool operator==(const FZEventCondition_Reputation& Other) const = default;
	bool operator!=(const FZEventCondition_Reputation& Other) const = default;
	
	virtual bool IsMet(const AZPlayerController* PC) const override;
	
	virtual FString ToString() const override;
	
	UPROPERTY(EditAnywhere)
	EZFaction Faction = static_cast<EZFaction>(0);
	
	UPROPERTY(EditAnywhere)
	bool bOver = true;
	
	UPROPERTY(EditAnywhere)
	float Threshold = 50.f;
};

USTRUCT()
struct ZOMBIES_API FZEventCondition_Inventory : public FZEventCondition
{
	GENERATED_BODY()
	
	virtual ~FZEventCondition_Inventory() override = default;
	bool operator==(const FZEventCondition_Inventory& Other) const = default;
	bool operator!=(const FZEventCondition_Inventory& Other) const = default;
	
	virtual bool IsMet(const AZPlayerController* PC) const override;
	
	virtual FString ToString() const override;

	UPROPERTY(EditAnywhere, meta = (AllowedTypes = "ZInventoryItemData"))
	FPrimaryAssetId ItemID;
	
	UPROPERTY(EditAnywhere)
	int32 Count = 1;
};

USTRUCT()
struct ZOMBIES_API FZEventCondition_Skill : public FZEventCondition
{
	GENERATED_BODY()
	
	virtual ~FZEventCondition_Skill() override = default;
	bool operator==(const FZEventCondition_Skill& Other) const = default;
	bool operator!=(const FZEventCondition_Skill& Other) const = default;
	
	virtual bool IsMet(const AZPlayerController* PC) const override;
	
	virtual FString ToString() const override;
	
	UPROPERTY(EditAnywhere)
	EZSkill RequiredSkill = static_cast<EZSkill>(0);
};

USTRUCT()
struct ZOMBIES_API FZEventReaction
{
	GENERATED_BODY()
	
	virtual ~FZEventReaction() = default;
	bool operator==(const FZEventReaction& Other) const = default;
	bool operator!=(const FZEventReaction& Other) const = default;

	virtual void Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const { unimplemented(); }
	virtual FString ToString() const {unimplemented(); return TEXT("Invalid");}
};

USTRUCT()
struct ZOMBIES_API FZEventReaction_ReceiveItem : public FZEventReaction
{
	GENERATED_BODY()
	
	virtual ~FZEventReaction_ReceiveItem() override = default;
	bool operator==(const FZEventReaction_ReceiveItem& Other) const = default;
	bool operator!=(const FZEventReaction_ReceiveItem& Other) const = default;

	virtual void Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const override;
	virtual FString ToString() const override;

	UPROPERTY(EditAnywhere, meta = (AllowedTypes = "ZInventoryItemData"))
	FPrimaryAssetId ItemID;
	
	UPROPERTY(EditAnywhere)
	int32 Count = 1;
};

USTRUCT()
struct ZOMBIES_API FZEventReaction_GiveItem : public FZEventReaction
{
	GENERATED_BODY()
	
	virtual ~FZEventReaction_GiveItem() override = default;
	bool operator==(const FZEventReaction_GiveItem& Other) const = default;
	bool operator!=(const FZEventReaction_GiveItem& Other) const = default;

	virtual void Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const override;
	virtual FString ToString() const override;

	UPROPERTY(EditAnywhere, meta = (AllowedTypes = "ZInventoryItemData"))
	FPrimaryAssetId ItemID;
	
	UPROPERTY(EditAnywhere)
	int32 Count = 1;
};

USTRUCT()
struct ZOMBIES_API FZEventReaction_GainXP : public FZEventReaction
{
	GENERATED_BODY()
	
	virtual ~FZEventReaction_GainXP() override = default;
	bool operator==(const FZEventReaction_GainXP& Other) const = default;
	bool operator!=(const FZEventReaction_GainXP& Other) const = default;

	virtual void Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const override;
	virtual FString ToString() const override;

	UPROPERTY(EditAnywhere)
	int32 XP = 100;
};

USTRUCT()
struct ZOMBIES_API FZEventReaction_Reputation : public FZEventReaction
{
	GENERATED_BODY()
	
	virtual ~FZEventReaction_Reputation() override = default;
	bool operator==(const FZEventReaction_Reputation& Other) const = default;
	bool operator!=(const FZEventReaction_Reputation& Other) const = default;

	virtual void Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const override;
	virtual FString ToString() const override;

	UPROPERTY(EditAnywhere)
	EZFaction Faction = static_cast<EZFaction>(0);
	
	UPROPERTY(EditAnywhere)
	float Change = 10.f;
};

USTRUCT()
struct ZOMBIES_API FZEventReaction_HomeTransform : public FZEventReaction
{
	GENERATED_BODY()
	
	virtual ~FZEventReaction_HomeTransform() override = default;
	bool operator==(const FZEventReaction_HomeTransform& Other) const = default;
	bool operator!=(const FZEventReaction_HomeTransform& Other) const = default;

	virtual void Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const override;
	virtual FString ToString() const override;

	UPROPERTY(EditAnywhere)
	FGameplayTag WaypointTag;
};

USTRUCT()
struct ZOMBIES_API FZEventReaction_StartPath : public FZEventReaction
{
	GENERATED_BODY()
	
	virtual ~FZEventReaction_StartPath() override = default;
	bool operator==(const FZEventReaction_StartPath& Other) const = default;
	bool operator!=(const FZEventReaction_StartPath& Other) const = default;

	virtual void Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const override;
	virtual FString ToString() const override;

	UPROPERTY(EditAnywhere)
	FGameplayTag PathTag;
	
	UPROPERTY(EditAnywhere)
	bool bEscortPlayer = false;
};

USTRUCT()
struct ZOMBIES_API FZEventReaction_CancelPath : public FZEventReaction
{
	GENERATED_BODY()
	
	virtual ~FZEventReaction_CancelPath() override = default;
	bool operator==(const FZEventReaction_CancelPath& Other) const = default;
	bool operator!=(const FZEventReaction_CancelPath& Other) const = default;

	virtual void Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const override;
	virtual FString ToString() const override;

	UPROPERTY(EditAnywhere)
	FGameplayTag PathTag;
};

USTRUCT()
struct ZOMBIES_API FZEventReaction_Attack : public FZEventReaction
{
	GENERATED_BODY()
	
	virtual ~FZEventReaction_Attack() override = default;
	bool operator==(const FZEventReaction_Attack& Other) const = default;
	bool operator!=(const FZEventReaction_Attack& Other) const = default;

	virtual void Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const override;
	virtual FString ToString() const override;
};

USTRUCT()
struct ZOMBIES_API FZEventReaction_TakeStolenItems : public FZEventReaction
{
	GENERATED_BODY()
	
	virtual ~FZEventReaction_TakeStolenItems() override = default;
	bool operator==(const FZEventReaction_TakeStolenItems& Other) const = default;
	bool operator!=(const FZEventReaction_TakeStolenItems& Other) const = default;

	virtual void Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const override;
	virtual FString ToString() const override;
};

USTRUCT()
struct ZOMBIES_API FZEventReaction_WarnAboutBreakingIn : public FZEventReaction
{
	GENERATED_BODY()
	
	virtual ~FZEventReaction_WarnAboutBreakingIn() override = default;
	bool operator==(const FZEventReaction_WarnAboutBreakingIn& Other) const = default;
	bool operator!=(const FZEventReaction_WarnAboutBreakingIn& Other) const = default;

	virtual void Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const override;
	virtual FString ToString() const override;
};

USTRUCT()
struct ZOMBIES_API FZEventRow : public FTableRowBase
{
	GENERATED_BODY()

	FZEventRow() {}

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ID;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag NPC;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer RequiredEvents;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer BlockedEvents;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TInstancedStruct<FZEventReaction>> OnAvailable;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TInstancedStruct<FZEventReaction>> OnUnavailable;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TInstancedStruct<FZEventReaction>> OnCompleted;
};

USTRUCT()
struct ZOMBIES_API FZDialogueOptionRow : public FZEventRow
{
	GENERATED_BODY()

	bool IsNPCInitiated() const { return Text.IsEmpty(); }

	bool DoesNPCRespond() const { return !Response.IsEmpty(); }

	UPROPERTY(EditDefaultsOnly)
	FText Text;

	UPROPERTY(EditDefaultsOnly)
	FText Response;

	UPROPERTY(EditDefaultsOnly)
	bool bLocksDialogue = false;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ContinuationFrom;

	UPROPERTY(EditDefaultsOnly)
	bool bEndsDialogue = false;

	UPROPERTY(EditDefaultsOnly)
	EZDialogueContext DialogueContext = static_cast<EZDialogueContext>(0);

	UPROPERTY(EditDefaultsOnly)
	bool bRepeatable = false;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TInstancedStruct<FZEventCondition>> Conditions;
};


