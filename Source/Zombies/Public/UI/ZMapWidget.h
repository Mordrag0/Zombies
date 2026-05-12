// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZActivatableWidget.h"
#include "ZDelegateConnection.h"
#include "ZMapWidget.generated.h"

class UCommonTextBlock;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZMapWidget : public UZActivatableWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

protected:

	UFUNCTION()
	void UpdateTime(float InTimeOfDay, int32 InDay);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Day;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Time;

	FZDelegateConnection OnUpdateTimeConnection;
};

