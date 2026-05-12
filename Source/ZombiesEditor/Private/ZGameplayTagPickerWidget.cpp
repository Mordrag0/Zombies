// Copyright 2026 Luka Markuš. All rights reserved.


#include "ZGameplayTagPickerWidget.h"
#include "SZGameplayTagPicker.h"
#include "SZGameplayTagPickerButton.h"

UZGameplayTagPickerWidget::UZGameplayTagPickerWidget()
{
	bMultiSelect = false;
}

void UZGameplayTagPickerWidget::SetSelection(const FGameplayTagContainer& NewSelection)
{
	Selection = NewSelection;
	if (TagPickerButton.IsValid())
	{
		TagPickerButton->SetSelection(NewSelection);
	}
}

void UZGameplayTagPickerWidget::SetRootTag(FGameplayTag NewRootTag)
{
	RootTag = NewRootTag;
	if (TagPickerButton.IsValid())
	{
		TagPickerButton->SetRootTag(NewRootTag);
	}
}

void UZGameplayTagPickerWidget::SetMultiSelect(bool bInMultiSelect)
{
	bMultiSelect = bInMultiSelect;
	if (TagPickerButton.IsValid())
	{
		TagPickerButton->SetMultiSelect(bInMultiSelect);
	}
}

void UZGameplayTagPickerWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	TagPickerButton.Reset();
}

TSharedRef<SWidget> UZGameplayTagPickerWidget::RebuildWidget()
{
	return SAssignNew(TagPickerButton, SZGameplayTagPickerButton)
		.RootTag(RootTag)
		.PreSelected(Selection)
		.bMultiSelect(bMultiSelect)
		.OnTagsChanged_Lambda([this](const FGameplayTagContainer& NewSelection)
		{
			Selection = NewSelection;
			OnTagSelectionChanged.Broadcast(NewSelection);
		});
}

