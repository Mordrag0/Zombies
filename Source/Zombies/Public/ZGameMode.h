// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZGameMode.generated.h"

class AZPlayerController;
class AZCharacter;
class AZCoins;
class UZCharacterProgressionData;

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AZGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual bool SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate = FCanUnpause()) override;
	virtual bool ClearPause() override;
	
	bool GetAllowPlayerFriendlyFire() const { return bAllowPlayerFriendlyFire; }

	bool GetAllowNPCFriendlyFire() const { return bAllowNPCFriendlyFire; }

	void OnCharacterKilled(AZCharacter* Victim, AController* DeathInstigator);

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly)
	bool bAllowPlayerFriendlyFire;

	UPROPERTY(EditDefaultsOnly)
	bool bAllowNPCFriendlyFire;

	UPROPERTY(EditDefaultsOnly)
	float DifficultyMultiplier;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UZCharacterProgressionData> ProgressionData;
};

