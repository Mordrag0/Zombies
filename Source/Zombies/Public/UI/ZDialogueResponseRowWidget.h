// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ZRowWidget.h"
#include "ZDialogueResponseRowWidget.generated.h"

struct FZDialogueOptionRow;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZDialogueResponseRowWidget : public UZRowWidget
{
	GENERATED_BODY()
	
public:
	void Init(const FZDialogueOptionRow* InRow);

protected:
	virtual void OnClicked() override;

	const FZDialogueOptionRow* Row;
};

