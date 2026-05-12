// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "ZGameplayStatics.generated.h"

namespace ETeamAttitude
{
	enum Type : int;
}

enum class EZMovementMode : uint8;
enum class EZFaction : uint8;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static bool IsHourBetween(int32 Hour, int32 From, int32 To);
	
	static FString GetAttitudeAsString(ETeamAttitude::Type Attitude);
	
	static EZMovementMode GetMovementMode(const FName& Name);
	
	static const FText& GetFactionText(EZFaction Faction);
};
