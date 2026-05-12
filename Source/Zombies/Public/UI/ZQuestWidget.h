// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "ZQuestWidget.generated.h"

struct FZQuestRow;
class UVerticalBox;
class UZQuestTitleWidget;
class AZGameState;
class UZQuestDescriptionWidget;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZQuestWidget : public UZActivatableWidget
{
	GENERATED_BODY()
	
public:
	void Refresh();

	void SelectQuest(const FZQuestRow* Quest);

protected:
	void FillBox(AZGameState* GS, UVerticalBox* Box, const FGameplayTagContainer& Quests);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZQuestTitleWidget> QuestTitleWidgetClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> OpenedQuests;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> CompletedQuests;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> FailedQuests;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> UnavailableQuests;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZQuestDescriptionWidget> QuestDescriptionWidget;
};

