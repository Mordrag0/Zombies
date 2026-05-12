// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZPath.h"
#include "DrawDebugHelpers.h"
#include "AI/ZWaypoint.h"
#include "ZGameState.h"

AZPath::AZPath()
{
	bNetLoadOnClient = false;
}

#if WITH_EDITOR
void AZPath::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdatePathVisualization();
}

void AZPath::UpdatePathVisualization()
{
	FlushPersistentDebugLines(GetWorld());

	if (!bShowPath)
	{
		return;
	}

	for (int32 Idx = 0; Idx < Waypoints.Num() - 1; ++Idx)
	{
		const AZWaypoint* From = Waypoints[Idx];
		const AZWaypoint* To = Waypoints[Idx + 1];
		if (From && To)
		{
			DrawDebugDirectionalArrow(GetWorld(), From->GetActorLocation(), To->GetActorLocation(), 50.f, FColor::Yellow, true, -1.f, 0, 2.f);
		}
	}
}

#endif

void AZPath::BeginPlay()
{
	Super::BeginPlay();

	AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
	if (!ensure(GS))
	{
		return;
	}
	GS->RegisterActor(GS->GetPaths(), this, PathTag);
}

void AZPath::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
	if (!ensure(GS))
	{
		return;
	}
	GS->UnregisterActor(GS->GetPaths(), PathTag);
}

