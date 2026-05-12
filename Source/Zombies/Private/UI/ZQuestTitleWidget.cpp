// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZQuestTitleWidget.h"
#include "UI/ZQuestWidget.h"
#include "Events/ZQuest.h"
#include "UI/ZButton.h"
#include "CommonTextBlock.h"

void UZQuestTitleWidget::Init(UZQuestWidget* InParentWidget, const FZQuestRow* InQuest)
{
	ParentWidget = InParentWidget;
	Quest = InQuest;

	Button->SetText(Quest->Title);
}

void UZQuestTitleWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button->OnClicked().AddUObject(this, &ThisClass::OnClicked);
}

void UZQuestTitleWidget::OnClicked()
{
	ParentWidget->SelectQuest(Quest);
}

