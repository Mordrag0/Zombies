// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ZRowWidget.h"
#include "ZDialogueRowWidget.generated.h"

class AZNPCharacter;
struct FZDialogueOptionRow;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZDialogueRowWidget : public UZRowWidget
{
	GENERATED_BODY()
	
public:
	void Init(const FZDialogueOptionRow* InRow);

protected:
	virtual void OnClicked() override;

	const FZDialogueOptionRow* Row;
	
};

