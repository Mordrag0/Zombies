// Copyright 2026 Luka Markuš. All rights reserved.


#include "SZGameplayTagPicker.h"
#include "Editor.h"
#include "GameplayTagsManager.h"
#include "ZEditorText.h"
#include "Brushes/SlateColorBrush.h"
#include "Components/VerticalBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "ZombiesEditor"

SZGameplayTagPicker::SZGameplayTagPicker()
{
	bMultiSelect = false;
	bStartExpanded = false;
	RootTag = FGameplayTag::EmptyTag;
}

void SZGameplayTagPicker::Construct(const FArguments& InArgs)
{
	RootTag = InArgs._RootTag;
	FilteredTags = InArgs._FilteredTags;
	Selection = InArgs._PreSelected;
	bMultiSelect = InArgs._bMultiSelect;
	bStartExpanded = InArgs._bStartExpanded;
	OnTagsChanged = InArgs._OnTagsChanged;
	OnClosed = InArgs._OnClosed;
	
	UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
	TSharedPtr<FGameplayTagNode> SelectedRootNode = GameplayTagsManager.FindTagNode(RootTag);
	if (SelectedRootNode.IsValid())
	{
		RootNodes = SelectedRootNode->GetChildTagNodes();
	}
	else
	{
		GameplayTagsManager.GetFilteredGameplayRootTags(FString(), RootNodes);
	}
	
	FilterNodes();
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromName(RootTag.GetTagName()))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(SearchBox, SSearchBox)
			.OnTextChanged(this, &SZGameplayTagPicker::OnSearchTextChanged)
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SNew(SScrollBox)
			.Orientation(Orient_Vertical)
			+ SScrollBox::Slot()
			[
				SAssignNew(TagTreeView, STreeView<TSharedPtr<FGameplayTagNode>>)
				.TreeItemsSource(&FilteredRootNodes)
				.SelectionMode(bMultiSelect ? ESelectionMode::Multi : ESelectionMode::SingleToggle)
				.OnGenerateRow_Lambda([this](TSharedPtr<FGameplayTagNode> Node,	const TSharedRef<STableViewBase>& OwnerTable)
				{
					return SNew(STableRow<TSharedPtr<FGameplayTagNode>>, OwnerTable)
							.ShowSelection(false) // disable built-in selection highlight
							[
								SNew(SBorder)
								.BorderImage_Lambda([this, Node]()
								{
									static FSlateColorBrush SelectedBrush(FLinearColor(0.2f, 0.5f, 0.2f, 1.0f));
									static FSlateColorBrush ParentBrush(FLinearColor(0.2f, 0.2f, 0.5f, 1.0f));
									static FSlateColorBrush NormalBrush(FLinearColor(0.1f, 0.1f, 0.1f, 1.0f));
								
									if (Selection.HasTagExact(Node->GetCompleteTag()))
									{
										return &SelectedBrush;
									}
									if (Selection.HasTag(Node->GetCompleteTag()))
									{
										return &ParentBrush;
									}
									else
									{
										return &NormalBrush;
									}
								})
								.BorderBackgroundColor_Lambda([this, Node]()
								{
									if (Selection.HasTagExact(Node->GetCompleteTag()))
									{
										return FLinearColor(0.2f, 0.5f, 0.2f, 1.0f);
									}
									else if (Selection.HasTag(Node->GetCompleteTag()))
									{
										return FLinearColor(0.2f, 0.2f, 0.5f, 1.0f);
									}
									else
									{
										return FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);
									}
								})
								.OnMouseButtonDown_Lambda([this, Node](const FGeometry&, const FPointerEvent&)
								{
									const FGameplayTag Tag = Node->GetCompleteTag();
									if (bMultiSelect)
									{
										if (Selection.HasTagExact(Tag))
										{
											Selection.RemoveTag(Tag);
										}
										else
										{
											Selection.AddTag(Tag);
										}
										OnTagsChanged.ExecuteIfBound(Selection);
									}
									else
									{
										if (Selection.HasTagExact(Tag))
										{
											Selection.RemoveTag(Tag);
										}
										else
										{
											Selection = Tag.GetSingleTagContainer();
										}
										OnTagsChanged.ExecuteIfBound(Selection);
										OnClosed.ExecuteIfBound();
									}
									TagTreeView->RequestTreeRefresh();
									return FReply::Handled();
								})
								[
									SNew(STextBlock)
									.Text(FText::FromName(Node->GetSimpleTagName()))
								]
							];
				})
				.OnGetChildren_Lambda([this](TSharedPtr<FGameplayTagNode> Node, TArray<TSharedPtr<FGameplayTagNode>>& NodeChildren)
				{
					OnGetChildren(Node, NodeChildren);
				})
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			bMultiSelect ?
				SNew(SButton)
				.OnClicked_Lambda([this]()
				{
					OnClosed.ExecuteIfBound();
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
					.Text(ZEditorText::OK)
				]
			: SNullWidget::NullWidget
		]
	];
	
	if (bStartExpanded)
	{
		for (TSharedPtr<FGameplayTagNode> RootNode : RootNodes)
		{
			ExpandAll(RootNode, true);
		}
	}
	else if (!Selection.IsEmpty())
	{
		for (TSharedPtr<FGameplayTagNode> RootNode : RootNodes)
		{
			ExpandSelected(RootNode);
		}
	}
	else
	{
		for (TSharedPtr<FGameplayTagNode> RootNode : RootNodes)
		{
			Expand(RootNode, InArgs._PreExpanded);
		}
	}
}

void SZGameplayTagPicker::SetSelection(const FGameplayTagContainer& NewSelection)
{
	Selection = NewSelection;
}

void SZGameplayTagPicker::FocusSearchBox()
{
	GEditor->GetTimerManager()->SetTimerForNextTick([this]()
	{
		if (SearchBox.IsValid())
		{
			FSlateApplication::Get().SetKeyboardFocus(SearchBox, EFocusCause::SetDirectly);
		}
	});
}

void SZGameplayTagPicker::OnSearchTextChanged(const FText& InSearchText)
{
	SearchText = InSearchText;
	SearchText.ToString().ParseIntoArray(SearchStrings, TEXT(" "), true);
	FilterNodes();
	if (!bStartExpanded)
	{
		for (TSharedPtr<FGameplayTagNode> RootNode : RootNodes)
        {
        	ExpandAll(RootNode, !SearchText.IsEmpty());
        }
	}
}

void SZGameplayTagPicker::ExpandAll(TSharedPtr<FGameplayTagNode> Node, bool bExpand)
{
	TagTreeView->SetItemExpansion(Node, bExpand);
	for (const TSharedPtr<FGameplayTagNode>& Child : Node->GetChildTagNodes())
	{
		ExpandAll(Child, bExpand);
	}
}

bool SZGameplayTagPicker::ExpandSelected(TSharedPtr<FGameplayTagNode> Node)
{
	bool bExpand = Selection.HasTagExact(Node->GetCompleteTag());
	for (const TSharedPtr<FGameplayTagNode>& Child : Node->GetChildTagNodes())
	{
		bExpand |= ExpandSelected(Child);
	}
	if (bExpand)
	{
		TagTreeView->SetItemExpansion(Node, true);
	}
	return bExpand;
}

bool SZGameplayTagPicker::Expand(TSharedPtr<FGameplayTagNode> Node, const FGameplayTagContainer& Expanded)
{
	bool bExpand = Expanded.HasTagExact(Node->GetCompleteTag());
	for (const TSharedPtr<FGameplayTagNode>& Child : Node->GetChildTagNodes())
	{
		bExpand |= Expand(Child, Expanded);
	}
	if (bExpand)
	{
		TagTreeView->SetItemExpansion(Node, true);
	}
	return bExpand;
}

void SZGameplayTagPicker::FilterNodes()
{
	FilteredRootNodes.Empty();
    
	for (const TSharedPtr<FGameplayTagNode>& RootNode : RootNodes)
	{
		if (NodeMatchesFilter(RootNode))
		{
			FilteredRootNodes.Add(RootNode);
		}
	}
    
	if (TagTreeView.IsValid())
	{
		TagTreeView->RequestTreeRefresh();
	}
}

bool SZGameplayTagPicker::NodeMatchesFilter(TSharedPtr<FGameplayTagNode> Node)
{
	for (const TSharedPtr<FGameplayTagNode>& Child : Node->GetChildTagNodes())
	{
		if (NodeMatchesFilter(Child))
		{
			return true;
		}
	}
	const FString NodeString = Node->GetCompleteTagString();
	for (const FString& SearchStr : SearchStrings)
	{
		if (!NodeString.Contains(SearchStr))
		{
			return false;
		}
	}
	if (!FilteredTags.IsEmpty() && !FilteredTags.HasTagExact(Node->GetCompleteTag()))
	{
		return false;
	}
	return true;
}

void SZGameplayTagPicker::OnGetChildren(TSharedPtr<FGameplayTagNode> Node, TArray<TSharedPtr<FGameplayTagNode>>& OutChildren)
{
	for (const TSharedPtr<FGameplayTagNode>& Child : Node->GetChildTagNodes())
	{
		if (NodeMatchesFilter(Child))
		{
			OutChildren.Add(Child);
		}
	}
}

#undef LOCTEXT_NAMESPACE

