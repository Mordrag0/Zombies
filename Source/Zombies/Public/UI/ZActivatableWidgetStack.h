// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "ZActivatableWidgetStack.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZActivatableWidgetStack : public UCommonActivatableWidgetStack
{
	GENERATED_BODY()

public:
	UZActivatableWidgetStack(const FObjectInitializer& Initializer);
	
	bool GetHasBackground() const { return bHasBackground; }

protected:
	virtual void OnWidgetAddedToList(UCommonActivatableWidget& AddedWidget) override;

	UPROPERTY(EditAnywhere)
	bool bHasBackground;
};

