// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZImpactEffect.generated.h"

UCLASS()
class ZOMBIES_API AZImpactEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	AZImpactEffect();

	void SetSurfaceHit(const FHitResult& InUseImpact);

protected:
	virtual void BeginPlay() override;

	FHitResult UseImpact;
};

