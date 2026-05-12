// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Widgets/Views/STreeView.h"
#include "Widgets/SCompoundWidget.h"

class SSearchBox;
/**
 * 
 */
class ZOMBIESEDITOR_API SZGameplayTagPicker : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FZOnTagsChanged, const FGameplayTagContainer&)
	DECLARE_DELEGATE(FZOnClosed)
	
	SZGameplayTagPicker();

	SLATE_BEGIN_ARGS(SZGameplayTagPicker)
		: _RootTag(FGameplayTag::EmptyTag)
		, _bMultiSelect(false)
		, _bStartExpanded(false)
		{}
		SLATE_ARGUMENT(FGameplayTag, RootTag)
		SLATE_ARGUMENT(FGameplayTagContainer, FilteredTags)
		SLATE_ARGUMENT(FGameplayTagContainer, PreSelected)
		SLATE_ARGUMENT(FGameplayTagContainer, PreExpanded)
		SLATE_ARGUMENT(bool, bMultiSelect)
		SLATE_ARGUMENT(bool, bStartExpanded)
		SLATE_EVENT(FZOnTagsChanged, OnTagsChanged)
		SLATE_EVENT(FZOnClosed, OnClosed)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void SetSelection(const FGameplayTagContainer& NewSelection);
	void FocusSearchBox();
	
private:
	void OnSearchTextChanged(const FText& InSearchText);
	void ExpandAll(TSharedPtr<FGameplayTagNode> Node, bool bExpand);
	bool ExpandSelected(TSharedPtr<FGameplayTagNode> Node);
	bool Expand(TSharedPtr<FGameplayTagNode> Node, const FGameplayTagContainer& Expanded);
	
	void FilterNodes();
	bool NodeMatchesFilter(TSharedPtr<FGameplayTagNode> Node);
	void OnGetChildren(TSharedPtr<FGameplayTagNode> Node, TArray<TSharedPtr<FGameplayTagNode>>& OutChildren);

	FGameplayTag RootTag;
	FGameplayTagContainer FilteredTags;
	FZOnTagsChanged OnTagsChanged;
	bool bMultiSelect;
	bool bStartExpanded;
	FGameplayTagContainer Selection;
	
	FZOnClosed OnClosed;
	
	FText SearchText;
	TArray<FString> SearchStrings;
	TArray<TSharedPtr<FGameplayTagNode>> RootNodes;
	TArray<TSharedPtr<FGameplayTagNode>> FilteredRootNodes;
	
	TSharedPtr<STreeView<TSharedPtr<FGameplayTagNode>>> TagTreeView;
	TSharedPtr<SSearchBox> SearchBox;
};

