// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZButton.h"
#include "CommonTextBlock.h"

void UZButton::SetText(const FText& InText)
{
	ButtonText->SetText(InText);
}

void UZButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetText(DefaultText);
}

void UZButton::NativePreConstruct()
{
	const bool bIsDesignTime = IsDesignTime();
	if (bIsDesignTime)
	{
		SetText(DefaultText);
	}
}

