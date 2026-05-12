// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ZGameInstance.generated.h"

enum class EZSkill : uint8;

USTRUCT()
struct FZSkillData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly)
	int32 SkillPointCost = 10;
};

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	FPrimaryAssetId GetCoinsID() const { return CoinsID; }

	FPrimaryAssetId GetLockpickID() const { return LockpickID; }

	const FZSkillData* GetSkillData(EZSkill Skill) const {return SkillData.Find(Skill); }

protected:
	UPROPERTY(EditDefaultsOnly)
	FPrimaryAssetId CoinsID;

	UPROPERTY(EditDefaultsOnly)
	FPrimaryAssetId LockpickID;

	UPROPERTY(EditDefaultsOnly)
	TMap<EZSkill, FZSkillData> SkillData;
};

