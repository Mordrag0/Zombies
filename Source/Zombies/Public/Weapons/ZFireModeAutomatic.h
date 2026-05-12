// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ZFireMode.h"
#include "ZFireModeAutomatic.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZFireModeAutomatic : public UZFireMode
{
	GENERATED_BODY()
	
public:
	virtual void FireComplete() override;

};

