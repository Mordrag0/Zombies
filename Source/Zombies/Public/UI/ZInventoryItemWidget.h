// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZInventoryItemWidget.generated.h"

class UCommonTextBlock;
class UImage;
class UZItemCountWidget;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZInventoryItemWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void Init(FPrimaryAssetId InItemId, int32 InCount, UZItemCountWidget* InCountWidget);

	virtual void CountSelected(int32 InCount);

	FPrimaryAssetId GetItemId() const { return ItemId; }

protected:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UImage> Image;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Count;

	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void Select();

	UPROPERTY()
	TObjectPtr<UZItemCountWidget> CountWidget;

	FPrimaryAssetId ItemId;

	int32 ItemCount;

	int32 Value;
};

