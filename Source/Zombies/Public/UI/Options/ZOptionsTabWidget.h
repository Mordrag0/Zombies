// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZOptionsTabWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZOptionsTabWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	bool IsDirty() const { return bDirty; }
	void MarkDirty() { bDirty = true; }
	virtual void Apply() {}
	virtual void Reset() {}

	FName GetTabID() const { return TabID; }
	FText GetTabDisplayName() const { return TabDisplayName; }

protected:
	bool bDirty;

	UPROPERTY(EditDefaultsOnly)
	FName TabID;

	UPROPERTY(EditDefaultsOnly)
	FText TabDisplayName;
};

