// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZItemCountWidget.generated.h"

class USlider;
class UCommonTextBlock;
class UZButton;
class UZInventoryItemWidget;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZItemCountWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:

	void Init(int32 InMax, UZInventoryItemWidget* InInventoryItemWidget);

protected:

	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void OnChanged(float InValue);

	UFUNCTION()
	void OnAccept();

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USlider> Slider;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Value;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UZButton> AcceptButton;

	int32 Max;

	int32 SelectedValue;

	UPROPERTY()
	TObjectPtr<UZInventoryItemWidget> InventoryItemWidget;
};

