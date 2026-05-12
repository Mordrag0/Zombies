// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZInventoryWidget.h"
#include "ZPlayerController.h"
#include "Characters/ZCharacter.h"
#include "Inventory/ZInventoryComponent.h"
#include "Components/UniformGridPanel.h"
#include "UI/ZInventoryItemWidget.h"
#include "Components/UniformGridSlot.h"
#include "CommonTextBlock.h"
#include "ZGameInstance.h"

UZInventoryWidget::UZInventoryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NumCols = 8;
}

void UZInventoryWidget::Refresh()
{
	if (!Inventory)
	{
		return;
	}

	InventoryList->ClearChildren();
	uint32 Idx = 0;
	UZGameInstance* GI = GetWorld()->GetGameInstance<UZGameInstance>();
	const FPrimaryAssetId CoinsID = GI->GetCoinsID();
	for (const FZInventoryEntry& InventorySlot : Inventory->GetInventory().Items)
	{
		if (InventorySlot.ItemId == CoinsID)
		{
			continue;
		}
		UZInventoryItemWidget* ItemWidget = CreateWidget<UZInventoryItemWidget>(this, ItemWidgetClass);
		ItemWidget->Init(InventorySlot.ItemId, InventorySlot.Count, CountWidget);
		UUniformGridSlot* UniformGridSlot = InventoryList->AddChildToUniformGrid(ItemWidget);
		UniformGridSlot->SetVerticalAlignment(VAlign_Fill);
		UniformGridSlot->SetHorizontalAlignment(HAlign_Fill);

		UniformGridSlot->SetColumn(Idx % NumCols);
		UniformGridSlot->SetRow(Idx / NumCols);

		Idx++;
	}

	for (; Idx < TotalCount; Idx++)
	{
		UCommonUserWidget* ItemWidget = CreateWidget<UCommonUserWidget>(this, EmptyItemWidgetClass);
		UUniformGridSlot* UniformGridSlot = InventoryList->AddChildToUniformGrid(ItemWidget);
		UniformGridSlot->SetVerticalAlignment(VAlign_Fill);
		UniformGridSlot->SetHorizontalAlignment(HAlign_Fill);

		UniformGridSlot->SetColumn(Idx % NumCols);
		UniformGridSlot->SetRow(Idx / NumCols);
	}
}

void UZInventoryWidget::Refresh(UZInventoryComponent* InInventory)
{
	Inventory = InInventory;

	Refresh();
}

