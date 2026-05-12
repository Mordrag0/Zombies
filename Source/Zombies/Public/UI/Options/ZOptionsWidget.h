// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ZActivatableWidget.h"
#include "ZOptionsWidget.generated.h"

class UZButton;
class UZOptionsTabWidget;
class UZGraphicsOptionsTabWidget;
class UZAudioOptionsTabWidget;
class UZInputOptionsTabWidget;
class UZGameplayOptionsTabWidget;
class UZUnsavedChangesWidget;
class UCommonTabListWidgetBase;
class UCommonAnimatedSwitcher;
class UZTabListWidget;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZOptionsWidget : public UZActivatableWidget // #ZTODO use GameSettings plugin from Lyra?
{
	GENERATED_BODY()
	
public:
	void Show();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void Accept();

	UFUNCTION()
	void Reset();

	UFUNCTION()
	void Back();

	UFUNCTION()
	void HandleTabSelected(FName TabID);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> AcceptButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> ResetButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZButton> BackButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZGameplayOptionsTabWidget> GameplayOptionsTab;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZGraphicsOptionsTabWidget> GraphicsOptionsTab;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZAudioOptionsTabWidget> AudioOptionsTab;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZInputOptionsTabWidget> InputOptionsTab;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZUnsavedChangesWidget> UnsavedChangesWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZTabListWidget> TabList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonAnimatedSwitcher> TabContent; // Switches between tab content

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZButton> TabButtonClass;

private:
	void RegisterTabs();
};

