// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "ZTabListWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZTabListWidget : public UCommonTabListWidgetBase
{
	GENERATED_BODY()
	
public:
	void SetTabDisplayNames(const TMap<FName, FText>& InNames);

protected:
	virtual void HandleTabCreation_Implementation(FName TabNameID, UCommonButtonBase* TabButton);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> TabButtonContainer;

	UPROPERTY()
	TMap<FName, FText> TabDisplayNames;
};

