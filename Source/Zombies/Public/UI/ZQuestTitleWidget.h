// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZQuestTitleWidget.generated.h"

class UZButton;
class UZQuestWidget;
struct FZQuestRow;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZQuestTitleWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(UZQuestWidget* InParentWidget, const FZQuestRow* InQuest);

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void OnClicked();

	UPROPERTY()
	TObjectPtr<UZQuestWidget> ParentWidget;

	const FZQuestRow* Quest;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> Button;
};

