// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZQuestDescriptionWidget.generated.h"

class UCommonTextBlock;
class UVerticalBox;
struct FZQuestRow;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZQuestDescriptionWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void Init(const FZQuestRow* Quest);

	void Clear();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Description;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> ObjectivesBox;
};

