// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZTradeRowWidget.h"
#include "Characters/ZNPCharacter.h"
#include "ZPlayerController.h"

void UZTradeRowWidget::Init(AZNPCharacter* InNPC)
{
	NPC = InNPC;
}

void UZTradeRowWidget::OnClicked()
{
	AZPlayerController* PC = GetOwningPlayer<AZPlayerController>();
	PC->Trade(NPC);
}

