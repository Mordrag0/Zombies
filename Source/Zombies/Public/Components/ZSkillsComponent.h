// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZSkillsComponent.generated.h"

enum class EZSkill : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FZOnSkillUnlocked, EZSkill);

UCLASS( ClassGroup=(Custom) )
class ZOMBIES_API UZSkillsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UZSkillsComponent();

	bool HasSkill(EZSkill Skill) const { return Skills.Contains(Skill); }

	void LearnSkill(EZSkill Skill);

	FZOnSkillUnlocked OnSkillUnlocked;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSet<EZSkill> Skills;
};

