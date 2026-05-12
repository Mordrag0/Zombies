// Copyright 2026 Luka MarkuÃ…Â¡ All rights reserved.


#include "ZGameMode.h"
#include "Events/ZEvent.h"
#include "Engine/DataTable.h"
#include "ZPlayerController.h"
#include "Characters/ZCharacter.h"
#include "ZGameState.h"
#include "Characters/ZCharacterProgressionData.h"
#include "Player/ZPlayerState.h"
#include "Components/ZLevelingComponent.h"
#include "Curves/CurveFloat.h"
#include "ZLog.h"

AZGameMode::AZGameMode()
{
	DifficultyMultiplier = 1.f;
}

void AZGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GetWorld()->IsPaused())
	{
		ClearPause();

		// #ZTODO: notify player that the game was unpaused
	}
}

bool AZGameMode::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
	const bool bResult = Super::SetPause(PC, CanUnpauseDelegate);
	UE_LOG(LogZGame, Log, TEXT("%s paused the game."), *PC->GetName());
	return bResult;
}

bool AZGameMode::ClearPause()
{
	const bool bResult = Super::ClearPause();
	UE_LOG(LogZGame, Log, TEXT("Game unpaused."));
	return bResult;
}

void AZGameMode::OnCharacterKilled(AZCharacter* Victim, AController* DeathInstigator)
{
	if (const FGameplayTag DeathEvent = Victim->GetDeathEvent(); DeathEvent.IsValid())
	{
		AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
		GS->CompleteEvent(DeathEvent, Victim, DeathInstigator);
	}
	const int32 VictimLevel = Victim->GetCharacterLevel();
	const float DeathXPReward = ProgressionData->XPRewardCurve->GetFloatValue(static_cast<float>(VictimLevel));
	if (DeathXPReward > 0.f)
	{
		AZPlayerState* PS = DeathInstigator ? DeathInstigator->GetPlayerState<AZPlayerState>() : nullptr;
		if (PS)
		{
			PS->GetLevelingComponent()->AddXP(DeathXPReward);
		}
	}
}

void AZGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AZGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


