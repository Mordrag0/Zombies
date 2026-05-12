// Copyright 2026 Luka Markuš. All rights reserved.


#include "Components/ZSkillsComponent.h"

UZSkillsComponent::UZSkillsComponent()
{
}

void UZSkillsComponent::LearnSkill(EZSkill Skill)
{
	if (Skills.Contains(Skill))
	{
		ensure(0);
		return;
	}
	Skills.Add(Skill);
	OnSkillUnlocked.Broadcast(Skill);
}


