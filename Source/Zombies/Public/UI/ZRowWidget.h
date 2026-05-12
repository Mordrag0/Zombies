// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZRowWidget.generated.h"

class UZButton;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZRowWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void SetText(const FText& InText);

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	virtual void OnClicked() { }

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> Button;
};

