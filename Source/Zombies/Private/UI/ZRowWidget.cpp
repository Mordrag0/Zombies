// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZRowWidget.h"
#include "UI/ZButton.h"

void UZRowWidget::SetText(const FText& InText)
{
	Button->SetText(InText);
}

void UZRowWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button->OnClicked().AddUObject(this, &ThisClass::OnClicked);
}

