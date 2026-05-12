// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ZInventoryItemWidget.h"
#include "ZLootItemWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZLootItemWidget : public UZInventoryItemWidget
{
	GENERATED_BODY()
	
protected:
	virtual void Select() override;
};

