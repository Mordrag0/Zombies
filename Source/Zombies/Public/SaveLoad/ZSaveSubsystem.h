// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ZSaveSubsystem.generated.h"

class UZSaveComponent;
class UZSaveGame;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void SaveGame(const FString& SlotName);
	void LoadGame(const FString& SlotName);
	
	void RegisterComponent(UZSaveComponent* SaveComponent);
	void UnregisterComponent(UZSaveComponent* SaveComponent);

protected:
	void SaveGameState(UZSaveGame* Save);
	void LoadGameState(UZSaveGame* Save);
	void CleanInvalidPointers();

	UPROPERTY()
	TArray<TWeakObjectPtr<UZSaveComponent>> RegisteredSaveComponents;
};

