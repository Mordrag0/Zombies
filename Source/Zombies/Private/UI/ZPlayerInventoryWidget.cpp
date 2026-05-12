// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZPlayerInventoryWidget.h"
#include "UI/ZInventoryWidget.h"
#include "CommonTextBlock.h"
#include "Inventory/ZInventoryComponent.h"

void UZPlayerInventoryWidget::Open(UZInventoryComponent* InInventory)
{
	Coins->SetText(FText::AsNumber(InInventory->GetCoins()));
	InventoryWidget->Refresh(InInventory);
}

