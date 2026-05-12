// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZLevelingComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FZOnLevelUp, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FZOnXPGained, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FZOnSkillPointsChanged, int32);

UCLASS( ClassGroup=(Custom))
class ZOMBIES_API UZLevelingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UZLevelingComponent();

	void AddXP(int32 XPToAdd);

	int32 GetLevel() const { return Level; }

	float GetXP() const { return XP; }

	float GetTotalXPForLevel(int32 InLevel) const;

	int32 GetSkillPoints() const { return SkillPoints; }

	void SpendSkillPoints(int32 Points);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FZOnXPGained OnXPGained;
	FZOnLevelUp OnLevelUp;
	FZOnSkillPointsChanged OnSkillPointsChanged;

protected:
	void LevelUp();

    UPROPERTY(EditDefaultsOnly, Category = "Leveling")
    float BaseXPPerLevel;

    UPROPERTY(EditDefaultsOnly, Category = "Leveling")
    float XPScalingFactor;

    UPROPERTY(EditDefaultsOnly, Category = "Leveling")
    int32 SkillPointsPerLevel;

	UPROPERTY(ReplicatedUsing = OnRep_XP)
	int32 XP;

	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UPROPERTY(ReplicatedUsing = OnRep_Level)
	int32 Level;

	UFUNCTION()
	void OnRep_Level();

	UPROPERTY(ReplicatedUsing = OnRep_SkillPoints)
	int32 SkillPoints;

	UFUNCTION()
	void OnRep_SkillPoints();
};

