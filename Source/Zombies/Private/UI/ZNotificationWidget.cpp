// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZNotificationWidget.h"
#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "UI/ZNotificationManagerWidget.h"

void UZNotificationWidget::Show(const FText& Message, FZNotificationStyle* Style)
{
	if (!ensure(Style))
	{
		return;
	}

	Text->SetText(Message);
	Text->SetStyle(Style->TextStyle);
	if (Style->Icon)
	{
		Icon->SetBrushFromTexture(Style->Icon);
		Icon->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Icon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

