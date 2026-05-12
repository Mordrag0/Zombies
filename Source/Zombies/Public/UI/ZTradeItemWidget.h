// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZInventoryItemWidget.h"
#include "Inventory/ZInventoryComponent.h"
#include "ZTradeItemWidget.generated.h"

class UImage;
class UCommonTextBlock;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZTradeItemWidget : public UZInventoryItemWidget
{
	GENERATED_BODY()
	
public:
	virtual void Init(FPrimaryAssetId InItemId, int32 InCount, UZItemCountWidget* InCountWidget) override;

	virtual void CountSelected(int32 InCount) override;

	int32 GetSelectedValue() const;

	int32 GetSelectedCount() const { return SelectedCount; }

protected:
	virtual void Select() override;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TradingCount;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TradingValue;

	int32 SelectedCount;
};

