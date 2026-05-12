// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZActivatableWidget.h"
#include "ZLockpickWidget.generated.h"

class AZLock;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZLockpickWidget : public UZActivatableWidget
{
	GENERATED_BODY()
	
public:
	void StartLockpicking(AZLock* InLock);

};

