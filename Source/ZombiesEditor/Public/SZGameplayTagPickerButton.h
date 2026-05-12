// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Widgets/SCompoundWidget.h"

class IMenu;
class SButton;
class SZGameplayTagPicker;

/**
 * 
 */
class ZOMBIESEDITOR_API SZGameplayTagPickerButton : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FZOnTagsChanged, const FGameplayTagContainer&)
	
	SZGameplayTagPickerButton();

	SLATE_BEGIN_ARGS(SZGameplayTagPickerButton)
		: _RootTag(FGameplayTag::EmptyTag)
		, _bMultiSelect(false)
		{}
	SLATE_ARGUMENT(FGameplayTag, RootTag)
	SLATE_ARGUMENT(FGameplayTagContainer, FilteredTags)
	SLATE_ARGUMENT(FGameplayTagContainer, PreSelected)
	SLATE_ARGUMENT(FGameplayTagContainer, PreExpanded)
	SLATE_ARGUMENT(bool, bMultiSelect)
	SLATE_EVENT(FZOnTagsChanged, OnTagsChanged)
SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
		
	void SetSelection(const FGameplayTagContainer& NewSelection);
	void SetRootTag(FGameplayTag InRootTag);
	void SetMultiSelect(bool bInMultiSelect);
	
private:
	TSharedPtr<SButton> Button;
	TSharedPtr<IMenu> GameplayPickerMenu;
	TSharedPtr<SZGameplayTagPicker> GameplayTagPicker;
	
	FGameplayTagContainer Selection;
	FGameplayTag RootTag;
	bool bMultiSelect;
	
	FZOnTagsChanged OnTagsChanged;
};
