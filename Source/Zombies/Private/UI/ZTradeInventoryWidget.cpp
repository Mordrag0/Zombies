// Copyright 2026 Luka Markuš. All rights reserved.

#include "UI/ZTradeInventoryWidget.h"
#include "Inventory/ZInventoryComponent.h"
#include "Components/TextBlock.h"
#include "UI/ZInventoryWidget.h"
#include "UI/ZTradeItemWidget.h"
#include "CommonTextBlock.h"

void UZTradeInventoryWidget::Refresh(UZInventoryComponent* InInventory)
{
	Coins->SetText(FText::AsNumber(InInventory->GetCoins()));
	InventoryWidget->Refresh(InInventory);
}

int32 UZTradeInventoryWidget::GetTradeCost() const
{
	int32 TotalCost = 0;

	const TArray<UZTradeItemWidget*> ItemWidgets = InventoryWidget->GetAllItemWidgets<UZTradeItemWidget>();
	for (UZTradeItemWidget* ItemWidget : ItemWidgets)
	{
		TotalCost += ItemWidget->GetSelectedValue();
	}

	return TotalCost;
}

