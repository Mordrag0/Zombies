// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZActivatableWidget.h"
#include "ZDelegateConnection.h"
#include "ZPauseMenuWidget.generated.h"

class UZOptionsWidget;
class UZButton;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZPauseMenuWidget : public UZActivatableWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void Resume();

	UFUNCTION()
	void Save();

	UFUNCTION()
	void Load();

	UFUNCTION()
	void ShowOptions();

	UFUNCTION()
	void Quit();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> ResumeButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> SaveButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> LoadButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> OptionsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> QuitButton;

	UPROPERTY()
	TObjectPtr<UZOptionsWidget> OptionsWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZOptionsWidget> OptionsWidgetClass;

	FZDelegateConnection OnOptionsWidgetDeactivatedConnection;
};

