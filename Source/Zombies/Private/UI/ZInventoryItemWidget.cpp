// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZInventoryItemWidget.h"
#include "Components/Image.h"
#include "Inventory/ZInventoryItem.h"
#include "CommonTextBlock.h"
#include "ZPlayerController.h"
#include "Characters/ZCharacter.h"
#include "UI/ZItemCountWidget.h"
#include "Engine/AssetManager.h"
#include "Inventory/ZInventoryItemData.h"
#include "UI/ZInventoryWidget.h"
#include "Inventory/ZInventoryComponent.h"
#include "UI/ZActivatableWidget.h"

void UZInventoryItemWidget::Init(FPrimaryAssetId InItemId, int32 InCount, UZItemCountWidget* InCountWidget)
{
	CountWidget = InCountWidget;
	ItemId = InItemId;
	ItemCount = InCount;

	const UAssetManager& Manager = UAssetManager::Get();
	const UZInventoryItemData* ItemData = Cast<UZInventoryItemData>(Manager.GetPrimaryAssetObject(InItemId));
	if (ensure(ItemData))
	{
		if(ensure(ItemData->Icon))
		{
			Image->SetBrushFromTexture(ItemData->Icon.Get());
		}
		Value = ItemData->Value;
	}
	Count->SetText(FText::AsNumber(InCount));
}

void UZInventoryItemWidget::CountSelected(int32 InCount)
{

}

FReply UZInventoryItemWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);

	Select();

	return FReply::Handled();
}

void UZInventoryItemWidget::Select()
{
	if (AZPlayerController* PC = GetOwningPlayer<AZPlayerController>())
	{
		if (AZCharacter* Character = PC->GetPawn<AZCharacter>())
		{
			Character->GetInventoryComponent()->Select(ItemId);
			if (UZActivatableWidget* ParentWidget = GetTypedOuter<UZActivatableWidget>())
			{
				ParentWidget->RemoveFromStack();
			}
		}
	}
}

