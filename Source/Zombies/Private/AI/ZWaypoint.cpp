// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZWaypoint.h"
#include "ZGameState.h"

AZWaypoint::AZWaypoint()
{
	bNetLoadOnClient = false;
	WaypointTag = FGameplayTag::EmptyTag;
}

void AZWaypoint::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}
	if (!WaypointTag.IsValid())
	{
		return;
	}
	AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
	if (!ensure(GS))
	{
		return;
	}
	GS->RegisterActor(GS->GetWaypoints(), this, WaypointTag);
}

void AZWaypoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (!WaypointTag.IsValid())
	{
		return;
	}
	AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
	if (!ensure(GS))
	{
		return;
	}
	GS->UnregisterActor(GS->GetWaypoints(), WaypointTag);
}

