// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZUnsavedChangesWidget.generated.h"

class UZButton;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZUnsavedChangesWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> AcceptButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> DiscardButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> CancelButton;
};

