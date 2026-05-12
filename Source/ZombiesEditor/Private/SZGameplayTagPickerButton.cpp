// Copyright 2026 Luka Markuš. All rights reserved.


#include "SZGameplayTagPickerButton.h"
#include "SZGameplayTagPicker.h"
#include "ZEditorText.h"
#include "ZEditorWidgetUtils.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Layout/WidgetPath.h"
#include "Widgets/Input/SButton.h"


SZGameplayTagPickerButton::SZGameplayTagPickerButton()
{
	bMultiSelect = false;
}

void SZGameplayTagPickerButton::Construct(const FArguments& InArgs)
{
	Selection = InArgs._PreSelected;
	RootTag = InArgs._RootTag;
	bMultiSelect = InArgs._bMultiSelect;
	OnTagsChanged = InArgs._OnTagsChanged;
	ChildSlot
	[
		SAssignNew(Button, SButton)
		.OnClicked_Lambda([this, InArgs]()
		{
			FMenuBuilder MenuBuilder(true, nullptr);
			MenuBuilder.AddWidget(
				SAssignNew(GameplayTagPicker, SZGameplayTagPicker)
					.RootTag(RootTag)
					.FilteredTags(InArgs._FilteredTags)
					.bMultiSelect(bMultiSelect)
					.PreSelected(Selection)
					.PreExpanded(InArgs._PreExpanded)
					.OnTagsChanged_Lambda([this](const FGameplayTagContainer& NewContainer)
					{
						Selection = NewContainer;
						OnTagsChanged.ExecuteIfBound(NewContainer);
					})
					.OnClosed_Lambda([this]()
					{
						if (GameplayPickerMenu.IsValid())
						{
							GameplayPickerMenu->Dismiss();
							GameplayPickerMenu.Reset();
						}
					})
				, FText::GetEmpty()
			);
			GameplayPickerMenu = FSlateApplication::Get().PushMenu(
				SharedThis(this),
				FWidgetPath(),
				MenuBuilder.MakeWidget(),
				FSlateApplication::Get().GetCursorPos(),
				FPopupTransitionEffect::ContextMenu
			);
			GameplayTagPicker->FocusSearchBox();
			return FReply::Handled();
		})
		[
			SNew(STextBlock)
			.Text_Lambda([this]()
			{
				if (Selection.IsEmpty())
				{
					return FText::FromName(FGameplayTag::EmptyTag.GetTagName());
				}
				FTextBuilder TextBuilder;
				for (const FGameplayTag SelectedTag : Selection)
				{
					TextBuilder.AppendLine(FText::FromName(SelectedTag.GetTagName()));
				}
				return TextBuilder.ToText();
			})
		]
	];
}

void SZGameplayTagPickerButton::SetSelection(const FGameplayTagContainer& NewSelection)
{
	Selection = NewSelection;
	if (GameplayTagPicker.IsValid())
	{
		GameplayTagPicker->SetSelection(NewSelection);
	}
}

void SZGameplayTagPickerButton::SetRootTag(FGameplayTag InRootTag)
{
	RootTag = InRootTag;
}

void SZGameplayTagPickerButton::SetMultiSelect(bool bInMultiSelect)
{
	bMultiSelect = bInMultiSelect;
}

