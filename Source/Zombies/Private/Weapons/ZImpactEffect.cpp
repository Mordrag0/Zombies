// Copyright 2026 Luka Markuš. All rights reserved.


#include "Weapons/ZImpactEffect.h"

AZImpactEffect::AZImpactEffect()
{

}

void AZImpactEffect::SetSurfaceHit(const FHitResult& InUseImpact)
{
	UseImpact = InUseImpact;
}

void AZImpactEffect::BeginPlay()
{
	Super::BeginPlay();
	
}


