// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZTeachRowWidget.h"
#include "ZPlayerController.h"
#include "CommonTextBlock.h"
#include "ZGameInstance.h"

void UZTeachRowWidget::Init(EZSkill InSkill)
{
	Skill = InSkill;

	if (UZGameInstance* GI = GetWorld()->GetGameInstance<UZGameInstance>())
	{
		if (const FZSkillData* SkillData = GI->GetSkillData(Skill))
		{
			SetText(SkillData->Name);
		}
	}
}

void UZTeachRowWidget::OnClicked()
{
	AZPlayerController* PC = GetOwningPlayer<AZPlayerController>();
	PC->OnLearnSkillSelected(Skill);
}

