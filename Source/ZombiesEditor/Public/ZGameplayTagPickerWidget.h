// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/Widget.h"
#include "ZGameplayTagPickerWidget.generated.h"

class SZGameplayTagPickerButton;

/**
 * 
 */
UCLASS()
class ZOMBIESEDITOR_API UZGameplayTagPickerWidget : public UWidget
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FZOnTagSelectionChanged, const FGameplayTagContainer&, NewSelection);

	UZGameplayTagPickerWidget();
	
	UFUNCTION(BlueprintCallable)
	void SetSelection(const FGameplayTagContainer& NewSelection);

	UFUNCTION(BlueprintCallable)
	void SetRootTag(FGameplayTag NewRootTag);

	UFUNCTION(BlueprintCallable)
	void SetMultiSelect(bool bInMultiSelect);

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	
	UPROPERTY(BlueprintAssignable)
	FZOnTagSelectionChanged OnTagSelectionChanged;
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	FGameplayTagContainer Selection;
	FGameplayTag RootTag;
	bool bMultiSelect;

private:
	TSharedPtr<SZGameplayTagPickerButton> TagPickerButton;
};

