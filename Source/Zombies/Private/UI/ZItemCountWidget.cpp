// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZItemCountWidget.h"
#include "Components/Slider.h"
#include "UI/ZInventoryItemWidget.h"
#include "UI/ZButton.h"
#include "CommonTextBlock.h"

void UZItemCountWidget::Init(int32 InMax, UZInventoryItemWidget* InInventoryItemWidget)
{
	Max = InMax;
	InventoryItemWidget = InInventoryItemWidget;
	Slider->SetValue(1);
	SelectedValue = Max;
}

void UZItemCountWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Slider->OnValueChanged.AddDynamic(this, &ThisClass::OnChanged);
	AcceptButton->OnClicked().AddUObject(this, &ThisClass::OnAccept);
}

void UZItemCountWidget::OnChanged(float InValue)
{
	SelectedValue = InValue * Max;
	Value->SetText(FText::AsNumber(SelectedValue));
}

void UZItemCountWidget::OnAccept()
{
	SetVisibility(ESlateVisibility::Hidden);
	InventoryItemWidget->CountSelected(SelectedValue);
}

