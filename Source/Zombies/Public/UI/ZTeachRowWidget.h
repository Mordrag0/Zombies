// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ZRowWidget.h"
#include "ZTeachRowWidget.generated.h"

enum class EZSkill : uint8;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZTeachRowWidget : public UZRowWidget
{
	GENERATED_BODY()
	
public:
	void Init(EZSkill InSkill);

protected:
	virtual void OnClicked() override;

	EZSkill Skill;
};

