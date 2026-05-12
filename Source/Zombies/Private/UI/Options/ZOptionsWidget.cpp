// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/Options/ZOptionsWidget.h"
#include "UI/ZButton.h"
#include "UI/Options/ZOptionsTabWidget.h"
#include "UI/Options/ZGameplayOptionsTabWidget.h"
#include "UI/Options/ZGraphicsOptionsTabWidget.h"
#include "UI/Options/ZAudioOptionsTabWidget.h"
#include "UI/Options/ZInputOptionsTabWidget.h"
#include "UI/Options/ZUnsavedChangesWidget.h"
#include "CommonTabListWidgetBase.h"
#include "CommonAnimatedSwitcher.h"
#include "UI/ZTabListWidget.h"

void UZOptionsWidget::Show()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UZOptionsWidget::NativeConstruct()
{
	RegisterTabs();
}

void UZOptionsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TabList->SetLinkedSwitcher(TabContent);
	TabList->OnTabSelected.AddDynamic(this, &ThisClass::HandleTabSelected);

	AcceptButton->OnClicked().AddUObject(this, &ThisClass::Accept);
	ResetButton->OnClicked().AddUObject(this, &ThisClass::Reset);
	BackButton->OnClicked().AddUObject(this, &ThisClass::Back);

	if (ensure(UnsavedChangesWidget))
	{
		//UnsavedChangesWidget->AcceptButton->OnClicked().AddUObject(this, &ThisClass::OnApplyAndSwitch);
		//UnsavedChangesWidget->DiscardButton->OnClicked().AddUObject(this, &ThisClass::OnDiscardAndSwitch);
		//UnsavedChangesWidget->CancelButton->OnClicked().AddUObject(this, &ThisClass::OnCancelSwitch);
	}
}

void UZOptionsWidget::Accept()
{

}

void UZOptionsWidget::Reset()
{

}

void UZOptionsWidget::Back()
{
	RemoveFromStack();
}

void UZOptionsWidget::HandleTabSelected(FName TabID)
{

}

void UZOptionsWidget::RegisterTabs()
{
	// Walk the switcher children and collect ID -> Name from each panel
	TMap<FName, FText> DisplayNames;
	FName FirstTabID = NAME_None;

	for (int32 Idx = 0; Idx < TabContent->GetChildrenCount(); ++Idx)
	{
		UZOptionsTabWidget* Child = Cast<UZOptionsTabWidget>(TabContent->GetChildAt(Idx));

		if (!ensure(Child))
		{
			continue;
		}

		const FName TabID = Child->GetTabID();
		const FText TabName = Child->GetTabDisplayName();

		if (!ensure(TabID != NAME_None))
		{
			continue;
		}
		DisplayNames.Emplace(TabID, TabName);

		if (FirstTabID == NAME_None)
		{
			FirstTabID = TabID;
		}
	}

	// Give the tab list the name map *before* registering so HandleTabCreation can use it
	TabList->SetTabDisplayNames(DisplayNames);

	// Register - now order doesn't matter, each child self-describes
	for (int32 Idx = 0; Idx < TabContent->GetChildrenCount(); ++Idx)
	{
		if (UZOptionsTabWidget* Child = Cast<UZOptionsTabWidget>(TabContent->GetChildAt(Idx)))
		{
			const FName TabID = Child->GetTabID();
			TabList->RegisterTab(TabID, TabButtonClass, Child);
		}
	}

	// Select the first valid tab
	if (FirstTabID != NAME_None)
	{
		TabList->SelectTabByID(FirstTabID);
	}
}


