// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZTradeInventoryWidget.generated.h"

class UZInventoryComponent;
class UZInventoryWidget;
class UCommonTextBlock;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZTradeInventoryWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void Refresh(UZInventoryComponent* InInventory);

	int32 GetTradeCost() const;

	UZInventoryWidget* GetInventoryWidget() const { return InventoryWidget; }

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Coins;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UZInventoryWidget> InventoryWidget;
};

