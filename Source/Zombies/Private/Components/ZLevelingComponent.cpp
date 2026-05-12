// Copyright 2026 Luka Markuš. All rights reserved.


#include "Components/ZLevelingComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

UZLevelingComponent::UZLevelingComponent()
{
	BaseXPPerLevel = 1000.f;
	XPScalingFactor = 1.5f;
	Level = 0;
	SkillPointsPerLevel = 10;

	SetIsReplicatedByDefault(true);
}

void UZLevelingComponent::AddXP(int32 XPToAdd)
{
	if (!ensure(GetOwner()->HasAuthority()))
	{
		return;
	}
	XP += XPToAdd;
	OnXPGained.Broadcast(XPToAdd);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, XP, this);

	while (XP >= GetTotalXPForLevel(Level + 1))
	{
		LevelUp();
	}
}

float UZLevelingComponent::GetTotalXPForLevel(int32 InLevel) const
{
	float Total = 0.f;

	for (int32 Idx = 1; Idx <= InLevel; ++Idx)
	{
        Total += BaseXPPerLevel * Idx * XPScalingFactor;
	}

	return Total;
}

void UZLevelingComponent::SpendSkillPoints(int32 Points)
{
	SkillPoints -= Points;
	OnSkillPointsChanged.Broadcast(SkillPoints);
}

void UZLevelingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_OwnerOnly;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, XP, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Level, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, SkillPoints, Params);
}

void UZLevelingComponent::LevelUp()
{
	Level++;
	OnLevelUp.Broadcast(Level);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Level, this);

	SkillPoints += SkillPointsPerLevel;
	OnSkillPointsChanged.Broadcast(SkillPoints);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, SkillPoints, this);
}

void UZLevelingComponent::OnRep_XP(int32 OldXP)
{
	OnXPGained.Broadcast(XP - OldXP);
}

void UZLevelingComponent::OnRep_Level()
{
	OnLevelUp.Broadcast(Level);
}

void UZLevelingComponent::OnRep_SkillPoints()
{
	OnSkillPointsChanged.Broadcast(SkillPoints);
}


