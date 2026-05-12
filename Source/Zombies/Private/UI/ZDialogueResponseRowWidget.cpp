// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZDialogueResponseRowWidget.h"
#include "CommonTextBlock.h"
#include "ZPlayerController.h"
#include "Events/ZEvent.h"

void UZDialogueResponseRowWidget::Init(const FZDialogueOptionRow* InRow)
{
	Row = InRow;
	SetText(Row->Response);
}

void UZDialogueResponseRowWidget::OnClicked()
{
	AZPlayerController* PC = GetOwningPlayer<AZPlayerController>();
	PC->OnResponseClicked(Row);
}

