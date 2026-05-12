// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GameplayTagContainer.h"
#include "ZWaypoint.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZWaypoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	AZWaypoint();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditInstanceOnly)
	FGameplayTag WaypointTag;
};

