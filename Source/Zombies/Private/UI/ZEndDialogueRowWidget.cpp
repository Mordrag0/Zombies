// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZEndDialogueRowWidget.h"
#include "ZPlayerController.h"

void UZEndDialogueRowWidget::OnClicked()
{
	AZPlayerController* PC = GetOwningPlayer<AZPlayerController>();
	PC->OnDialogueFinished();
}

