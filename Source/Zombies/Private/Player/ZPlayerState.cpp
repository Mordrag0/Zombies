// Copyright 2026 Luka Markuš. All rights reserved.


#include "Player/ZPlayerState.h"
#include "Components/ZLevelingComponent.h"
#include "Components/ZSkillsComponent.h"
#include "ZGameInstance.h"

AZPlayerState::AZPlayerState()
{
	LevelingComponent = CreateDefaultSubobject<UZLevelingComponent>(TEXT("Leveling component"));
	SkillsComponent = CreateDefaultSubobject<UZSkillsComponent>(TEXT("Skills component"));
}

bool AZPlayerState::Learn(EZSkill Skill)
{
	UZGameInstance* GI = GetWorld()->GetGameInstance<UZGameInstance>();
	if (!GI)
	{
		return false;
	}
	const FZSkillData* SkillData = GI->GetSkillData(Skill);
	if (!ensure(SkillData))
	{
		return false;
	}
	if (LevelingComponent->GetSkillPoints() < SkillData->SkillPointCost)
	{
		return false;
	}
	if (SkillsComponent->HasSkill(Skill))
	{
		ensure(0);
		return false;
	}
	LevelingComponent->SpendSkillPoints(SkillData->SkillPointCost);
	SkillsComponent->LearnSkill(Skill);
	return true;
}

