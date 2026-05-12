// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ZRowWidget.h"
#include "ZTradeRowWidget.generated.h"

class AZNPCharacter;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZTradeRowWidget : public UZRowWidget
{
	GENERATED_BODY()
	
public:
	void Init(AZNPCharacter* InNPC);

protected:
	virtual void OnClicked() override;

	UPROPERTY()
	TObjectPtr<AZNPCharacter> NPC;
};

