// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZActivatableWidget.h"
#include "ZSkillWidget.generated.h"

class UCommonTextBlock;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZSkillWidget : public UZActivatableWidget
{
	GENERATED_BODY()
	
public:
	void Refresh();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> SkillPoints;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> XP;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> XPToNextLevel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Level;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Lockpicking;
};

