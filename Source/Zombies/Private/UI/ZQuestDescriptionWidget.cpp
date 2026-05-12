// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZQuestDescriptionWidget.h"
#include "CommonTextBlock.h"
#include "Events/ZQuest.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"

void UZQuestDescriptionWidget::Init(const FZQuestRow* Quest)
{
	Title->SetText(Quest->Title);
	Description->SetText(Quest->Description);

	ObjectivesBox->ClearChildren();
	for (const FZQuestObjective& QuestObjective : Quest->Objectives)
	{
		UCommonTextBlock* QuestObjectiveText = WidgetTree->ConstructWidget<UCommonTextBlock>(UCommonTextBlock::StaticClass());
		QuestObjectiveText->SetText(QuestObjective.Description);
		UVerticalBoxSlot* VBSlot = ObjectivesBox->AddChildToVerticalBox(QuestObjectiveText);
		VBSlot->SetHorizontalAlignment(HAlign_Fill);
		VBSlot->SetVerticalAlignment(VAlign_Fill);
	}
}

void UZQuestDescriptionWidget::Clear()
{
	Title->SetText(FText::GetEmpty());
	Description->SetText(FText::GetEmpty());
	ObjectivesBox->ClearChildren();
}

