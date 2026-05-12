// Copyright 2026 Luka Markuš. All rights reserved.


#include "Level/ZLevelSettings.h"
#include "ZGameState.h"
#include "Engine/DirectionalLight.h"

AZLevelSettings::AZLevelSettings()
{
}

void AZLevelSettings::BeginPlay()
{
	Super::BeginPlay();
	
	if (AZGameState* GS = GetWorld()->GetGameState<AZGameState>())
	{
		if (ensure(SunLight))
		{
			GS->SetSunLight(SunLight);
		}
		if (MoonLight)
		{
			GS->SetMoonLight(MoonLight);
		}
	}
}

