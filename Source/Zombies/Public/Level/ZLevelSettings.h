// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZLevelSettings.generated.h"

class ADirectionalLight;

UCLASS()
class ZOMBIES_API AZLevelSettings : public AActor
{
	GENERATED_BODY()
	
public:	
	AZLevelSettings();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, Category = "Time of day")
	TObjectPtr<ADirectionalLight> SunLight;

	UPROPERTY(EditInstanceOnly, Category = "Time of day")
	TObjectPtr<ADirectionalLight> MoonLight;
};

