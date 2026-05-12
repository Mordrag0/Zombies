// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZSkillWidget.h"
#include "Player/ZPlayerState.h"
#include "CommonTextBlock.h"
#include "Components/ZLevelingComponent.h"
#include "Components/ZSkillsComponent.h"
#include "UI/ZText.h"
#include "ZTypes.h"

void UZSkillWidget::Refresh()
{
	AZPlayerState* ZPS = GetOwningPlayerState<AZPlayerState>();
	if (!ensure(ZPS))
	{
		return;
	}

	UZLevelingComponent* LevelingComponent = ZPS->GetLevelingComponent();
	SkillPoints->SetText(FText::AsNumber(LevelingComponent->GetSkillPoints()));
	XP->SetText(FText::AsNumber(LevelingComponent->GetXP()));
	XPToNextLevel->SetText(FText::AsNumber(LevelingComponent->GetTotalXPForLevel(LevelingComponent->GetLevel() + 1)));
	Level->SetText(FText::AsNumber(LevelingComponent->GetLevel()));

	UZSkillsComponent* SkillsComponent = ZPS->GetSkillsComponent();
	Lockpicking->SetText(SkillsComponent->HasSkill(EZSkill::Lockpicking) ? ZText::Yes : ZText::No);
}


