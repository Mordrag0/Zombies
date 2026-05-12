// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZLootItemWidget.h"
#include "ZGameplayStatics.h"
#include "UI/ZLootInventoryWidget.h"
#include "UI/ZLootWidget.h"

void UZLootItemWidget::Select()
{
	UZLootInventoryWidget* LootInventoryWidget = GetTypedOuter<UZLootInventoryWidget>();
	if (ensure(LootInventoryWidget))
	{
		EZLootWidgetType Type = LootInventoryWidget->GetType();
		UZLootWidget* LootWidget = GetTypedOuter<UZLootWidget>();
		LootWidget->Move(ItemId, ItemCount, Type);
	}
}

