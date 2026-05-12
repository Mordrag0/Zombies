// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZTradeItemWidget.h"
#include "Components/Image.h"
#include "Inventory/ZInventoryItem.h"
#include "CommonTextBlock.h"
#include "UI/ZItemCountWidget.h"
#include "ZPlayerController.h"
#include "UI/ZHUDWidget.h"
#include "Engine/AssetManager.h"
#include "Inventory/ZInventoryItemData.h"

void UZTradeItemWidget::Init(FPrimaryAssetId InItemId, int32 InCount, UZItemCountWidget* InCountWidget)
{
	Super::Init(InItemId, InCount, InCountWidget);

	UAssetManager& Manager = UAssetManager::Get();
	UZInventoryItemData* ItemData = Cast<UZInventoryItemData>(Manager.GetPrimaryAssetObject(InItemId));

	TradingCount->SetText(FText::AsNumber(0));
	if (ensure(ItemData))
	{
		TradingValue->SetText(FText::AsNumber(ItemData->Value));
	}
	SelectedCount = 0;
}

void UZTradeItemWidget::CountSelected(int32 InCount)
{
	SelectedCount = InCount;
	TradingCount->SetText(FText::AsNumber(InCount));

	AZPlayerController* PC = GetOwningPlayer<AZPlayerController>();
	PC->GetHUDWidget()->UpdateTradeCost(); // #ZTODO: does this need to go through the player controller?
}

int32 UZTradeItemWidget::GetSelectedValue() const
{
	return SelectedCount * Value;
}

void UZTradeItemWidget::Select()
{
	if (ensure(CountWidget))
	{
		CountWidget->SetVisibility(ESlateVisibility::Visible);
		CountWidget->Init(ItemCount, this);
	}
}

