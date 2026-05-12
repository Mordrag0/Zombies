// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "ZPath.generated.h"

class AZWaypoint;

UCLASS()
class ZOMBIES_API AZPath : public AActor
{
	GENERATED_BODY()
	
public:	
	AZPath();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	const TArray<AZWaypoint*>& GetWaypoints() const { return Waypoints; }

	const AZWaypoint* GetWaypoint(int32 Index) const { return Waypoints.IsValidIndex(Index) ? Waypoints[Index] : nullptr; }

	FGameplayTag GetPathTag() const { return PathTag; }
	FGameplayTag GetEventTag() const { return EventTag; }

protected:
#if WITH_EDITOR
	void UpdatePathVisualization();
#endif

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditInstanceOnly)
	TArray<TObjectPtr<AZWaypoint>> Waypoints;

	UPROPERTY(EditInstanceOnly)
	FGameplayTag PathTag;
	
	UPROPERTY(EditInstanceOnly)
	FGameplayTag EventTag;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly)
	bool bShowPath;
#endif
};

