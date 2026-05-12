// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZRowWidget.h"
#include "ZEndDialogueRowWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZEndDialogueRowWidget : public UZRowWidget
{
	GENERATED_BODY()
	
public:
protected:
	virtual void OnClicked() override;
};

