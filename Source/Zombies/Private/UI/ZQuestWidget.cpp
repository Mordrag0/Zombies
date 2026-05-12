// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZQuestWidget.h"
#include "Events/ZQuest.h"
#include "ZGameState.h"
#include "UI/ZQuestTitleWidget.h"
#include "UI/ZQuestDescriptionWidget.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/VerticalBox.h"

void UZQuestWidget::Refresh()
{
	AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
	if (!GS)
	{
		return;
	}

	FillBox(GS, OpenedQuests, GS->GetOpenedQuests());
	FillBox(GS, CompletedQuests, GS->GetCompletedQuests());
	FillBox(GS, FailedQuests, GS->GetFailedQuests());
	FillBox(GS, UnavailableQuests, GS->GetUnavailableQuests());

	QuestDescriptionWidget->Clear();
}

void UZQuestWidget::SelectQuest(const FZQuestRow* Quest)
{
	if (!ensure(QuestDescriptionWidget))
	{
		return;
	}
	QuestDescriptionWidget->Init(Quest);
}

void UZQuestWidget::FillBox(AZGameState* GS, UVerticalBox* Box, const FGameplayTagContainer& Quests)
{
	Box->ClearChildren();
	if (!ensure(QuestTitleWidgetClass))
	{
		return;
	}
	for (FGameplayTag Quest : Quests)
	{
		UZQuestTitleWidget* QuestTitleWidget = CreateWidget<UZQuestTitleWidget>(this, QuestTitleWidgetClass);
		QuestTitleWidget->Init(this, GS->GetQuest(Quest));
		UVerticalBoxSlot* VBSlot = Box->AddChildToVerticalBox(QuestTitleWidget);
		VBSlot->SetHorizontalAlignment(HAlign_Fill);
		VBSlot->SetVerticalAlignment(VAlign_Fill);
	}
}

