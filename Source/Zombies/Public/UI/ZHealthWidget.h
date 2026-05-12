// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZDelegateConnection.h"
#include "ZHealthWidget.generated.h"

class UProgressBar;
class AZFPCharacter;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZHealthWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(AZFPCharacter* Character);

protected:
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnHealthChanged(float Health, float MaxHealth);

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	FZDelegateConnection OnHealthChangedConnection;
};

