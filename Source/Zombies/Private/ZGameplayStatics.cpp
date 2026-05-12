// Copyright 2026 Luka Markuš. All rights reserved.


#include "ZGameplayStatics.h"
#include "GenericTeamAgentInterface.h"
#include "ZConstants.h"
#include "ZTypes.h"
#include "UI/ZText.h"

bool UZGameplayStatics::IsHourBetween(int32 Hour, int32 From, int32 To)
{
	if (From <= To)
	{
		return (Hour >= From) && (Hour < To);
	}
	else
	{
		return (Hour >= From) || (Hour < To);
	}
}

FString UZGameplayStatics::GetAttitudeAsString(ETeamAttitude::Type Attitude)
{
	switch (Attitude)
	{
	case ETeamAttitude::Friendly:
		return TEXT("Friendly");
	case ETeamAttitude::Neutral:
		return TEXT("Neutral");
	case ETeamAttitude::Hostile:
		return TEXT("Hostile");
	}
	return TEXT("Invalid");
}

EZMovementMode UZGameplayStatics::GetMovementMode(const FName& Name)
{
	static const TMap<FName, EZMovementMode> MovementModeMap = {
		{ ZMovementMode::Walking, EZMovementMode::Walking },
		{ ZMovementMode::Falling, EZMovementMode::Falling },
		{ ZMovementMode::Flying, EZMovementMode::Flying },
		{ ZMovementMode::Swimming, EZMovementMode::Swimming },
		{ ZMovementMode::Sliding, EZMovementMode::Sliding },
	};

	return MovementModeMap[Name];
}

const FText& UZGameplayStatics::GetFactionText(EZFaction Faction)
{
	switch (Faction) {
	case EZFaction::NoFaction:
		return ZText::NoFaction;
	case EZFaction::Player:
		return ZText::PlayerFaction;
	case EZFaction::Zombies:
		return ZText::ZombiesFaction;
	case EZFaction::FactionA:
		return ZText::FactionA;
	case EZFaction::FactionB:
		return ZText::FactionB;
	case EZFaction::FactionC:
		return ZText::FactionC;
	case EZFaction::FactionD:
		return ZText::FactionD;
	case EZFaction::MAX:
	default:
		return ZText::Invalid;
	}
}

