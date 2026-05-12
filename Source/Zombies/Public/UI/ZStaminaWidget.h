// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZDelegateConnection.h"
#include "ZStaminaWidget.generated.h"

class UZStaminaComponent;
class UProgressBar;
class AZFPCharacter;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZStaminaWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(AZFPCharacter* Character);

protected:
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnStaminaChanged(float Stamina, float MaxStamina);

	UPROPERTY()
	TObjectPtr<UZStaminaComponent> StaminaComponent;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;

	FZDelegateConnection OnStaminaChangedConnection;
};

