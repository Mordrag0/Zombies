// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZLootInventoryWidget.h"
#include "Inventory/ZInventoryComponent.h"
#include "UI/ZInventoryWidget.h"

void UZLootInventoryWidget::Refresh(UZInventoryComponent* Inventory)
{
	InventoryWidget->Refresh(Inventory);
}

void UZLootInventoryWidget::Refresh()
{
	InventoryWidget->Refresh();
}


