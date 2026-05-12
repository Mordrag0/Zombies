// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ZActivatableWidget.h"
#include "ZPlayerInventoryWidget.generated.h"

class UCommonTextBlock;
class UZInventoryWidget;
class UZInventoryComponent;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZPlayerInventoryWidget : public UZActivatableWidget
{
	GENERATED_BODY()
	
public:
	void Open(UZInventoryComponent* InInventory);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Coins;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZInventoryWidget> InventoryWidget;
};

