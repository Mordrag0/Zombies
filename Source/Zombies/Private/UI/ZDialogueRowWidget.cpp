// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZDialogueRowWidget.h"
#include "Events/ZEvent.h"
#include "ZPlayerController.h"

void UZDialogueRowWidget::Init(const FZDialogueOptionRow* InRow)
{
	Row = InRow;
	SetText(Row->Text);
}

void UZDialogueRowWidget::OnClicked()
{
	AZPlayerController* PC = GetOwningPlayer<AZPlayerController>();
	PC->OnDialogueOptionSelected(Row);
}

