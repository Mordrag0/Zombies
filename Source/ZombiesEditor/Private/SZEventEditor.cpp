// Copyright 2026 Luka Markuš. All rights reserved.


#include "SZEventEditor.h"

#include "DataTableEditorUtils.h"
#include "Editor.h"
#include "EngineStats.h"
#include "FileHelpers.h"
#include "GameplayTagsEditorModule.h"
#include "ZEventEditorSettings.h"
#include "Engine/DataTable.h"
#include "Widgets/Input/SSearchBox.h"
#include "GameplayTagsManager.h"
#include "GameplayTagsModule.h"
#include "ISettingsModule.h"
#include "PropertyCustomizationHelpers.h"
#include "SGameplayTagPicker.h"
#include "SZButton.h"
#include "SZGameplayTagPicker.h"
#include "SZGameplayTagPickerButton.h"
#include "ZEditorText.h"
#include "ZEditorWidgetUtils.h"
#include "ZGameplayTags.h"
#include "ZTypes.h"
#include "Engine/AssetManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Inventory/ZInventoryItemData.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "UI/ZText.h"
#include "UObject/SavePackage.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"

#define LOCTEXT_NAMESPACE "ZombiesEditor"
DEFINE_LOG_CATEGORY(LogZEventEditor)

FText GetEventTypeText(EZEventType EventType)
{
	switch (EventType)
	{
	case EZEventType::Regular:
		return ZEditorText::RegularEvent;
	case EZEventType::Dialogue:
		return ZEditorText::DialogueEvent;
	case EZEventType::Timed:
		return ZEditorText::TimedEvent;
	case EZEventType::Simple:
	default:
		return ZEditorText::SimpleEvent;
	}
}

TSharedPtr<FGameplayTagNode> GetNextNode(FGameplayTag Tag, FGameplayTag Parent)
{
	const UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
	if (TSharedPtr<FGameplayTagNode> ParentNode = GameplayTagsManager.FindTagNode(Parent))
	{
		TSharedPtr<FGameplayTagNode>* ChildNode = ParentNode->GetChildTagNodes().FindByPredicate([Tag](const TSharedPtr<FGameplayTagNode>& Node)
		{
			return Tag.MatchesTag(Node->GetCompleteTag());
		});
		return ChildNode ? *ChildNode : nullptr;
	}
	return  nullptr;
}

void FZNewEvent::Reset()
{
	RootTag = FGameplayTag::EmptyTag;
	TagName = FText::GetEmpty();
	Type = EZEventType::Simple;
	DataTable = nullptr;
}

SZEventEditor::SZEventEditor()
{
	bEditMode = false;
	bShowRepeatable = true;
	bShowIndirect = false;
	bShowPreviewFullNames = true;

	RelatedEvents.Emplace(EZEventRelation::Required);
	RelatedEvents.Emplace(EZEventRelation::Blocked);
	RelatedEvents.Emplace(EZEventRelation::RequiredBy);
	RelatedEvents.Emplace(EZEventRelation::BlockedBy);
	CurrentDirectlyRelatedEvents.Emplace(EZEventRelation::Required);
	CurrentDirectlyRelatedEvents.Emplace(EZEventRelation::Blocked);
	CurrentDirectlyRelatedEvents.Emplace(EZEventRelation::RequiredBy);
	CurrentDirectlyRelatedEvents.Emplace(EZEventRelation::BlockedBy);
	CurrentIndirectlyRelatedEvents.Emplace(EZEventRelation::Required);
	CurrentIndirectlyRelatedEvents.Emplace(EZEventRelation::Blocked);
	CurrentIndirectlyRelatedEvents.Emplace(EZEventRelation::RequiredBy);
	CurrentIndirectlyRelatedEvents.Emplace(EZEventRelation::BlockedBy);
	
	NewEventRootTag = ZGameplayTags::Event;
	
	for (int32 Idx = 0; Idx < static_cast<uint8>(EZEventType::MAX); ++Idx)
	{
		EventTypes.Emplace(static_cast<EZEventType>(Idx));
	}
	
	ConditionData =
	{
		{ FZEventCondition_Inventory::StaticStruct(), ZEditorText::Condition_Inventory, [this](TSharedPtr<FInstancedStruct> Struct)
			{ return BuildEditableEventInventoryConditionWidget(Struct); }},
		{ FZEventCondition_Reputation::StaticStruct(), ZEditorText::Condition_Reputation, [this](TSharedPtr<FInstancedStruct> Struct)
			{ return BuildEditableEventReputationConditionWidget(Struct); }},
		{ FZEventCondition_Skill::StaticStruct(), ZEditorText::Condition_Skill, [this](TSharedPtr<FInstancedStruct> Struct)
			{ return BuildEditableEventSkillConditionWidget(Struct); }},
	};
	ReactionData = 
	{
		{ FZEventReaction_ReceiveItem::StaticStruct(), ZEditorText::Reaction_ReceiveItem, [this](TSharedPtr<FInstancedStruct> Struct, EZEventReactionType Type)
			{ return BuildEditableEventReactionWidget_ReceiveItem(Struct, Type); }},
		{ FZEventReaction_GiveItem::StaticStruct(), ZEditorText::Reaction_GiveItem, [this](TSharedPtr<FInstancedStruct> Struct, EZEventReactionType Type)
			{ return BuildEditableEventReactionWidget_GiveItem(Struct, Type); }},
		{ FZEventReaction_GainXP::StaticStruct(), ZEditorText::Reaction_GainXP, [this](TSharedPtr<FInstancedStruct> Struct, EZEventReactionType Type)
			{ return BuildEditableEventReactionWidget_GainXP(Struct, Type); }},
		{ FZEventReaction_Reputation::StaticStruct(), ZEditorText::Condition_Reputation, [this](TSharedPtr<FInstancedStruct> Struct, EZEventReactionType Type)
			{ return BuildEditableEventReactionWidget_Reputation(Struct, Type); }},
		{ FZEventReaction_HomeTransform::StaticStruct(), ZEditorText::Reaction_HomeTransform, [this](TSharedPtr<FInstancedStruct> Struct, EZEventReactionType Type)
			{ return BuildEditableEventReactionWidget_HomeTransform(Struct, Type); }},
		{ FZEventReaction_StartPath::StaticStruct(), ZEditorText::Reaction_StartPath, [this](TSharedPtr<FInstancedStruct> Struct, EZEventReactionType Type)
			{ return BuildEditableEventReactionWidget_StartPath(Struct, Type); }},
		{ FZEventReaction_CancelPath::StaticStruct(), ZEditorText::Reaction_CancelPath, [this](TSharedPtr<FInstancedStruct> Struct, EZEventReactionType Type)
			{ return BuildEditableEventReactionWidget_CancelPath(Struct, Type); }},
		{ FZEventReaction_Attack::StaticStruct(), ZEditorText::Reaction_Attack, nullptr },
		{ FZEventReaction_TakeStolenItems::StaticStruct(), ZEditorText::Reaction_TakeStolenItems, nullptr },
		{ FZEventReaction_WarnAboutBreakingIn::StaticStruct(), ZEditorText::Reaction_WarnAboutBreakingIn, nullptr },
		{ FZEventReaction_StartTimedEvent::StaticStruct(), ZEditorText::Reaction_StartTimedEvent, [this](TSharedPtr<FInstancedStruct> Struct, EZEventReactionType Type)
			{ return BuildEditableEventReactionWidget_StartTimedEvent(Struct, Type); }},
		{ FZEventReaction_SetCanTrade::StaticStruct(), ZEditorText::Reaction_SetCanTrade, [this](TSharedPtr<FInstancedStruct> Struct, EZEventReactionType Type)
			{ return BuildEditableEventReactionWidget_SetCanTrade(Struct, Type); }},
	};
}

void SZEventEditor::Construct(const FArguments& InArgs)
{
	LoadEvents();

	ChildSlot
	[
		SNew(SSplitter)
		.Orientation(Orient_Horizontal)

		// Left panel - toolbar + search + list
		+ SSplitter::Slot()
		.Value(0.3)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 4.0f, 0.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("Refresh", "Refresh"))
					.OnClicked(this, &SZEventEditor::OnRefresh)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SCheckBox)
					.IsChecked(this, &SZEventEditor::IsRepeatableFilterChecked)
					.OnCheckStateChanged(this, &SZEventEditor::OnRepeatableFilterCheckStateChanged)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("RepeatableFilterLabel", "Show Repeatable"))
					]
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(SearchBox, SSearchBox)
				.OnTextChanged_Lambda([this](const FText&)
				{
					RefreshLists();
				})
			]
			+ SVerticalBox::Slot()
			[
				SAssignNew(ListScrollBox, SScrollBox)
				+ SScrollBox::Slot()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SExpandableArea)
						.AreaTitle(LOCTEXT("MiscEvents", "Misc Events"))
						.InitiallyCollapsed(false)
						.BodyContent()
						[
							SAssignNew(EventListView, SListView<TSharedPtr<FGameplayTag>>)
							.ListItemsSource(&FilteredEvents)
							.OnGenerateRow(this, &SZEventEditor::OnGenerateEventRow)
							.OnSelectionChanged(this, &SZEventEditor::OnEventSelectedFromList)
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SExpandableArea)
						.AreaTitle(LOCTEXT("DialogueEvents", "Dialogue Events"))
						.InitiallyCollapsed(false)
						.BodyContent()
						[
							SAssignNew(DialogueOptionListView, SListView<TSharedPtr<FGameplayTag>>)
							.ListItemsSource(&FilteredDialogueOptions)
							.OnGenerateRow(this, &SZEventEditor::OnGenerateEventRow)
							.OnSelectionChanged(this, &SZEventEditor::OnEventSelectedFromList)
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SExpandableArea)
						.AreaTitle(LOCTEXT("TimedEvents", "Timed Events"))
						.InitiallyCollapsed(false)
						.BodyContent()
						[
							SAssignNew(TimedEventListView, SListView<TSharedPtr<FGameplayTag>>)
							.ListItemsSource(&FilteredTimedEvents)
							.OnGenerateRow(this, &SZEventEditor::OnGenerateEventRow)
							.OnSelectionChanged(this, &SZEventEditor::OnEventSelectedFromList)
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SExpandableArea)
						.AreaTitle(LOCTEXT("SimpleEvents", "Simple Events"))
						.InitiallyCollapsed(false)
						.BodyContent()
						[
							SAssignNew(SimpleEventListView, SListView<TSharedPtr<FGameplayTag>>)
							.ListItemsSource(&FilteredSimpleEvents)
							.OnGenerateRow(this, &SZEventEditor::OnGenerateEventRow)
							.OnSelectionChanged(this, &SZEventEditor::OnEventSelectedFromList)
						]
					]
				]
			]
		]
		+ SSplitter::Slot()
		.Value(0.7)
		[
			SNew(SSplitter)
			.Orientation(Orient_Vertical)
			+ SSplitter::Slot()
			.Value(0.8)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.f)
					[
						SNullWidget::NullWidget
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(LOCTEXT("RemoveTags", "Remove tags"))
						.OnClicked_Lambda([this]()
						{
							FMenuBuilder MenuBuilder(true, nullptr);
							MenuBuilder.AddWidget(BuildRemoveTagsWidget(), FText::GetEmpty());
						
							ShowPopupMenu(MenuBuilder);
							return FReply::Handled();
						})
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(LOCTEXT("ValidateAllEvents", "Validate all events"))
						.OnClicked_Lambda([this]()
						{
							ValidateAll();
							return FReply::Handled();
						})
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(LOCTEXT("ValidateSelectedEvent", "Validate selected event"))
						.OnClicked_Lambda([this]()
						{
							Validate(CurrentEvent);
							return FReply::Handled();
						})
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(LOCTEXT("OpenTagManager", "Open Tag Manager"))
						.OnClicked_Lambda([this]()
						{
							FGameplayTagManagerWindowArgs Args;
							Args.bRestrictedTags = false;
							Args.HighlightedTag = CurrentEvent;
							UE::GameplayTags::Editor::OpenGameplayTagManager(Args);
							return FReply::Handled();
						})
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.ButtonStyle(FAppStyle::Get(), "NoBorder")
						.OnClicked_Lambda([]()
						{
							FModuleManager::LoadModuleChecked<ISettingsModule>("Settings")
								.ShowViewer("Project", "Project", "Event Editor");
							return FReply::Handled();
						})
						[
							SNew(SImage)
							.Image(FAppStyle::GetBrush("Icons.Settings"))
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					// + SHorizontalBox::Slot()
					// .AutoWidth()
					// [
					// 	SAssignNew(InsertButton, SButton)
					// 	.Text(LOCTEXT("AddTag", "Add event tag"))
					// 	.OnClicked(this, &SZEventEditor::OnInsertEventTag)
					// ]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(InsertButton, SButton)
						.Text(LOCTEXT("AddEvent", "Add event"))
						.OnClicked(this, &SZEventEditor::OnInsert)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(EditButton, SButton)
						.Text(ZEditorText::Edit)
						.OnClicked(this, &SZEventEditor::OnEdit)
						.Visibility(EVisibility::Collapsed)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(RemoveButton, SButton)
						.Text(ZEditorText::Remove)
						.OnClicked(this, &SZEventEditor::RemoveEvent)
						.Visibility(EVisibility::Collapsed)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(RevertButton, SButton)
						.Text(ZEditorText::Revert)
						.OnClicked(this, &SZEventEditor::OnRevert)
						.Visibility(EVisibility::Collapsed)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(SaveButton, SButton)
						.Text(ZEditorText::Save)
						.OnClicked(this, &SZEventEditor::Save)
						.Visibility(EVisibility::Collapsed)
					]
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.f)
				[
					SNew(SScrollBox)
					.Orientation(Orient_Vertical)
					+ SScrollBox::Slot()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SAssignNew(ContentArea, SBox)
							[
								SNew(STextBlock)
								.Text(FText::FromString("Select an event."))
							]
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SSeparator)
							.Orientation(Orient_Horizontal)
							.Thickness(6.f)
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SAssignNew(DetailsArea, SBox)
							[
								SNullWidget::NullWidget
							]
						]
					]
				]
			]
			+ SSplitter::Slot()
			.Value(0.2)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SCheckBox)
					.IsChecked(this, &SZEventEditor::IsShowPreviewFullNamesChecked)
					.OnCheckStateChanged(this, &SZEventEditor::OnShowPreviewFullNamesCheckStateChanged)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("RepeatableFilterLabel", "Show full names"))
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SSeparator)
					.Orientation(Orient_Horizontal)
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.f)
				[
					SAssignNew(PreviewArea, SBox)
					[
						SNullWidget::NullWidget
					]
				]
			]
		]
	];
	
	RefreshLists();
}

void SZEventEditor::UpdateListSelection()
{
	TSharedPtr<FGameplayTag>* FoundEvent = FilteredEvents.FindByPredicate(
		[this](const TSharedPtr<FGameplayTag>& Item)
		{
			return Item.IsValid() && *Item == CurrentEvent;
		});
	EventListView->SetSelection(FoundEvent ? *FoundEvent : nullptr, ESelectInfo::Direct);
	TSharedPtr<FGameplayTag>* FoundDialogueEvent = FilteredDialogueOptions.FindByPredicate(
		[this](const TSharedPtr<FGameplayTag>& Item)
		{
			return Item.IsValid() && *Item == CurrentEvent;
		});
	DialogueOptionListView->SetSelection(FoundDialogueEvent ? *FoundDialogueEvent : nullptr, ESelectInfo::Direct);
	TSharedPtr<FGameplayTag>* FoundSimpleEvent = FilteredSimpleEvents.FindByPredicate(
		[this](const TSharedPtr<FGameplayTag>& Item)
		{
			return Item.IsValid() && *Item == CurrentEvent;
		});
	SimpleEventListView->SetSelection(FoundSimpleEvent ? *FoundSimpleEvent : nullptr, ESelectInfo::Direct);
	TSharedPtr<FGameplayTag>* FoundTimedEvent = FilteredTimedEvents.FindByPredicate(
		[this](const TSharedPtr<FGameplayTag>& Item)
		{
			return Item.IsValid() && *Item == CurrentEvent;
		});
	TimedEventListView->SetSelection(FoundTimedEvent ? *FoundTimedEvent : nullptr, ESelectInfo::Direct);
}

void SZEventEditor::NavigateTo(FGameplayTag EventTag)
{
	CurrentEvent = EventTag;

	UpdateListSelection();
	
	if (EventTag.IsValid())
	{
		auto GetCurrentTagEvents = [this](
			FGameplayTagContainer& DirectContainer,
			FGameplayTagContainer& IndirectContainer,
			FGameplayTag Event,
			TFunction<FGameplayTagContainer(FGameplayTag, bool)> GetEvents)
		{
			DirectContainer = GetEvents(Event, false);
			IndirectContainer = GetEvents(Event, true);
			IndirectContainer.RemoveTags(DirectContainer);
		};

		GetCurrentTagEvents(CurrentDirectlyRelatedEvents[EZEventRelation::Required], CurrentIndirectlyRelatedEvents[EZEventRelation::Required], CurrentEvent,
							[this](FGameplayTag Event, bool bIndirect) { return GetRequiredEvents(Event, bIndirect); });
		GetCurrentTagEvents(CurrentDirectlyRelatedEvents[EZEventRelation::Blocked], CurrentIndirectlyRelatedEvents[EZEventRelation::Blocked], CurrentEvent,
							[this](FGameplayTag Event, bool bIndirect) { return GetBlockedEvents(Event, bIndirect); });
		GetCurrentTagEvents(CurrentDirectlyRelatedEvents[EZEventRelation::RequiredBy], CurrentIndirectlyRelatedEvents[EZEventRelation::RequiredBy], CurrentEvent,
							[this](FGameplayTag Event, bool bIndirect) { return GetRequiredByEvents(Event, bIndirect); });
		GetCurrentTagEvents(CurrentDirectlyRelatedEvents[EZEventRelation::BlockedBy], CurrentIndirectlyRelatedEvents[EZEventRelation::BlockedBy], CurrentEvent,
							[this](FGameplayTag Event, bool bIndirect) { return GetBlockedByEvents(Event, bIndirect); });

		if (CurrentDirectlyRelatedEvents[EZEventRelation::Required].HasTagExact(CurrentEvent))
		{
			UE_LOG(LogZEventEditor, Error, TEXT("Event %s requires itself"), *CurrentEvent.ToString());
		}
		if (CurrentDirectlyRelatedEvents[EZEventRelation::Blocked].HasTagExact(CurrentEvent))
		{
			UE_LOG(LogZEventEditor, Error, TEXT("Event %s blocks itself"), *CurrentEvent.ToString());
		}
		if (CurrentDirectlyRelatedEvents[EZEventRelation::RequiredBy].HasTagExact(CurrentEvent))
		{
			UE_LOG(LogZEventEditor, Error, TEXT("Event %s is required by itself"), *CurrentEvent.ToString());
		}
		if (CurrentDirectlyRelatedEvents[EZEventRelation::BlockedBy].HasTagExact(CurrentEvent))
		{
			UE_LOG(LogZEventEditor, Error, TEXT("Event %s is blocked by itself"), *CurrentEvent.ToString());
		}
		if (CurrentIndirectlyRelatedEvents[EZEventRelation::Required].HasTagExact(CurrentEvent))
		{
			UE_LOG(LogZEventEditor, Error, TEXT("Event %s indirectly requires itself"), *CurrentEvent.ToString());
		}
		if (CurrentIndirectlyRelatedEvents[EZEventRelation::Blocked].HasTagExact(CurrentEvent))
		{
			UE_LOG(LogZEventEditor, Error, TEXT("Event %s indirectly blocks itself"), *CurrentEvent.ToString());
		}
		if (CurrentIndirectlyRelatedEvents[EZEventRelation::RequiredBy].HasTagExact(CurrentEvent))
		{
			UE_LOG(LogZEventEditor, Error, TEXT("Event %s is indirectly required by itself"), *CurrentEvent.ToString());
		}
		if (CurrentIndirectlyRelatedEvents[EZEventRelation::BlockedBy].HasTagExact(CurrentEvent))
		{
			UE_LOG(LogZEventEditor, Error, TEXT("Event %s is indirectly blocked by itself"), *CurrentEvent.ToString());
		}
	}
	else
	{
		CurrentDirectlyRelatedEvents[EZEventRelation::Required].Reset();
		CurrentDirectlyRelatedEvents[EZEventRelation::Blocked].Reset();
		CurrentDirectlyRelatedEvents[EZEventRelation::RequiredBy].Reset();
		CurrentDirectlyRelatedEvents[EZEventRelation::BlockedBy].Reset();
		CurrentIndirectlyRelatedEvents[EZEventRelation::Required].Reset();
		CurrentIndirectlyRelatedEvents[EZEventRelation::Blocked].Reset();
		CurrentIndirectlyRelatedEvents[EZEventRelation::RequiredBy].Reset();
		CurrentIndirectlyRelatedEvents[EZEventRelation::BlockedBy].Reset();
	}
	RebuildContentArea();
	RebuildDetailsArea();
	PreviewArea->SetContent(SNullWidget::NullWidget);
	EditButton->SetVisibility(bEditMode ? EVisibility::Collapsed : EVisibility::Visible);
}

void SZEventEditor::RebuildContentArea()
{
	ContentArea->SetContent(BuildEventView());
}

void SZEventEditor::RebuildDetailsArea()
{
	DetailsArea->SetContent(BuildDialogueView());
}

TSharedRef<SWidget> SZEventEditor::BuildEventView()
{
	if (!CurrentEvent.IsValid())
	{
		return SNew(STextBlock)
				.Text(FText::FromString("No event selected."));
	}

	const EZEventType CurrentEventType = GetEventType(CurrentEvent);
	bool bRegularEvent = false;
	if (CurrentEventType == EZEventType::Regular || CurrentEventType == EZEventType::Dialogue)
	{
		EditedEvent = *GetEventRow(CurrentEvent);
		bRegularEvent = true;
	}
	
	TSharedRef<SVerticalBox> VBox = SNew(SVerticalBox);
	VBox->AddSlot()
	.AutoHeight()
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			bRegularEvent
				? BuildEditableGameplayTagWidget(EditedEvent.NPC, ZGameplayTags::NPC, EZEditEventType::Current, 20)
				: SNullWidget::NullWidget
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromName(CurrentEvent.GetTagName()))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 22))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SCheckBox)
			.IsChecked(this, &SZEventEditor::IsIndirectFilterChecked)
			.OnCheckStateChanged(this, &SZEventEditor::OnIndirectFilterCheckStateChanged)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("IndirectFilterLabel", "Show indirect events"))
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			GetEventsAffecting()
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			GetEventsAffectedBy()
		]
	];
	
	if (bRegularEvent)
	{
		VBox->AddSlot()
	   .FillHeight(1.f)
	   [
		   SNew(SVerticalBox)
		   + SVerticalBox::Slot()
		   .AutoHeight()
		   [
			   SNew(SHorizontalBox)
			   + SHorizontalBox::Slot()
			   .AutoWidth()
			   [
				   BuildDetailsPropertyNameWidget(LOCTEXT("OnAvailableReactions", "On available reactions:"))
			   ]
			   + SHorizontalBox::Slot()
			   .AutoWidth()
			   [
				   BuildEditableEventReactionsWidget(EZEventReactionType::OnAvailable)
			   ]
		   ]
		   + SVerticalBox::Slot()
		   .AutoHeight()
		   [
			   SNew(SHorizontalBox)
			   + SHorizontalBox::Slot()
			   .AutoWidth()
			   [
				   BuildDetailsPropertyNameWidget(LOCTEXT("OnUnavailableReactions", "On unavailable reactions:"))
			   ]
			   + SHorizontalBox::Slot()
			   .AutoWidth()
			   [
				   BuildEditableEventReactionsWidget(EZEventReactionType::OnUnavailable)
			   ]
		   ]
		   + SVerticalBox::Slot()
		   .AutoHeight()
		   [
			   SNew(SHorizontalBox)
			   + SHorizontalBox::Slot()
			   .AutoWidth()
			   [
				   BuildDetailsPropertyNameWidget(LOCTEXT("OnCompletedReactions", "On completed reactions:"))
			   ]
			   + SHorizontalBox::Slot()
			   .AutoWidth()
			   [
				   BuildEditableEventReactionsWidget(EZEventReactionType::OnCompleted)
			   ]
		   ]
	   ];
	}
	return VBox;
}

TSharedRef<SWidget> SZEventEditor::BuildDetailsPropertyNameWidget(const FText& PropertyName)
{
	return SNew(SBox)
			.WidthOverride(200.f)
			[
				SNew(STextBlock)
				.Text(PropertyName)
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableTextWidget(FText& Text, EZEditEventType Type)
{
	if (bEditMode)
	{
		return SNew(SEditableTextBox)
				.Text(Text)
				.OnTextCommitted_Lambda([this, &Text, Type](const FText& InText, ETextCommit::Type)
				{
					Text = InText;
					OnValueEdited(Type);
				});
	}
	else
	{
		return SNew(STextBlock)
				.Text(Text);
	}
}

TSharedRef<SWidget> SZEventEditor::BuildCheckboxWidget(bool bValue)
{
	return SNew(STextBlock)
			.Text(bValue ? ZText::Yes : ZText::No)
			.ColorAndOpacity(bValue ? FLinearColor::Green : FSlateColor::UseForeground());
}

TSharedRef<SWidget> SZEventEditor::BuildEditableCheckboxWidget(bool& bVariable, EZEditEventType Type)
{
	if (bEditMode)
	{
		return SNew(SCheckBox)
				.IsChecked_Lambda([&bVariable]()
				{
					return bVariable ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
				})
				.OnCheckStateChanged_Lambda([this, &bVariable, Type](ECheckBoxState NewState)
				{
					bVariable = NewState == ECheckBoxState::Checked;
					OnValueEdited(Type);
				});
	}
	else
	{
		return BuildCheckboxWidget(bVariable);
	}
}

TSharedRef<SWidget> SZEventEditor::BuildEditableGameplayTagWidget(FGameplayTag& Event, FGameplayTag ParentTag, EZEditEventType Type, int32 FontSize)
{
	if (bEditMode)
	{
		return SNew(SZGameplayTagPickerButton)
				.RootTag(ParentTag)
				.PreSelected(Event.GetSingleTagContainer())
				.OnTagsChanged_Lambda([this, &Event, Type](const FGameplayTagContainer& NewTag)
				{
					Event = (NewTag.Num() > 0) ? NewTag.First() : FGameplayTag::EmptyTag;
					OnValueEdited(Type);
				});
	}
	else
	{
		return SNew(STextBlock)
			.Text(Event.IsValid() ? FText::FromName(Event.GetTagName()) : FText::GetEmpty())
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", FontSize));
	}
}

TSharedRef<SWidget> SZEventEditor::CreateReactionTypeVBox(EZEventReactionType Type)
{
	TSharedRef<SVerticalBox> VBox = SNew(SVerticalBox);
	for (const FZEventReactionData& Reaction : ReactionData)
	{
		VBox->AddSlot()
		.AutoHeight()
		[
			SNew(SButton)
			.OnClicked_Lambda([this, Reaction, Type]()
			{
				TSharedPtr<FInstancedStruct> NewReaction = MakeShared<FInstancedStruct>();
				NewReaction->InitializeAs(Reaction.Type);
				GetEventReactionList(Type).Add(NewReaction);
				GetEditedEventReactionList(Type).AddDefaulted();
				OnEventReactionEdited(NewReaction, Type);
                
				GetEventReactionListView(Type)->RequestListRefresh();
				HidePopupMenu();
				
				return FReply::Handled();
			})
			[
				SNew(STextBlock).Text(Reaction.Text)
			]
		];
	}
	return VBox;
}

TSharedRef<SWidget> SZEventEditor::BuildRemoveReactionButton(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type)
{
	return SNew(SBox)
			.Padding(6.f, 0.f, 0.f, 0.f)
			[
				SNew(SButton)
					.OnClicked_Lambda([this, Reaction, Type]()
					{
						const int32 Index = GetEventReactionList(Type).IndexOfByKey(Reaction);
						if (ensure(Index != INDEX_NONE))
						{
							GetEventReactionList(Type).RemoveAt(Index);
							GetEditedEventReactionList(Type).RemoveAt(Index);
							OnCurrentEventEdited();
							EventConditionListView->RequestListRefresh();
						}
						return FReply::Handled();
					})
					[
						SNew(SImage)
						.Image(FAppStyle::GetBrush("GenericCommands.Delete"))
					]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventReactionWidget_ReceiveItem(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type)
{
	FZEventReaction_ReceiveItem* ReceiveItemReaction = Reaction->GetMutablePtr<FZEventReaction_ReceiveItem>();
	return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SObjectPropertyEntryBox)
                    .AllowedClass(UZInventoryItemData::StaticClass())
                    .ObjectPath_Lambda([ReceiveItemReaction]()
                    {
                    	FAssetData AssetData;
                    	UAssetManager::Get().GetPrimaryAssetData(ReceiveItemReaction->ItemID, AssetData);
                    	return AssetData.GetObjectPathString();
                    })
                    .OnObjectChanged_Lambda([this, Reaction, ReceiveItemReaction, Type](const FAssetData& AssetData)
                    {
                    	ReceiveItemReaction->ItemID = AssetData.GetPrimaryAssetId();
                    	OnEventReactionEdited(Reaction, Type);
                    })
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SNumericEntryBox<int32>)
					.AllowSpin(false)
					.Value_Lambda([this, ReceiveItemReaction]()
					{
						return TOptional<int32>(ReceiveItemReaction->Count);
					})
					.OnValueCommitted_Lambda([this, Reaction, ReceiveItemReaction, Type](int32 NewValue, ETextCommit::Type)
					{
						ReceiveItemReaction->Count = NewValue;
						OnEventReactionEdited(Reaction, Type);
					})
				]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventReactionWidget_GiveItem(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type)
{
	FZEventReaction_GiveItem* GiveItemReaction = Reaction->GetMutablePtr<FZEventReaction_GiveItem>();
	return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SObjectPropertyEntryBox)
					.AllowedClass(UZInventoryItemData::StaticClass())
					.ObjectPath_Lambda([GiveItemReaction]()
					{
						FAssetData AssetData;
						UAssetManager::Get().GetPrimaryAssetData(GiveItemReaction->ItemID, AssetData);
						return AssetData.GetObjectPathString();
					})
					.OnObjectChanged_Lambda([this, Reaction, GiveItemReaction, Type](const FAssetData& AssetData)
					{
						GiveItemReaction->ItemID = AssetData.GetPrimaryAssetId();
						OnEventReactionEdited(Reaction, Type);
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SNumericEntryBox<int32>)
					.AllowSpin(false)
					.Value_Lambda([this, GiveItemReaction]()
					{
						return TOptional<int32>(GiveItemReaction->Count);
					})
					.OnValueCommitted_Lambda([this, Reaction, GiveItemReaction, Type](int32 NewValue, ETextCommit::Type)
					{
						GiveItemReaction->Count = NewValue;
						OnEventReactionEdited(Reaction, Type);
					})
				]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventReactionWidget_GainXP(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type)
{
	FZEventReaction_GainXP* GainXPReaction = Reaction->GetMutablePtr<FZEventReaction_GainXP>();
	return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SNumericEntryBox<int32>)
					.AllowSpin(false)
					.Value_Lambda([this, GainXPReaction]()
					{
						return TOptional<int32>(GainXPReaction->XP);
					})
					.OnValueCommitted_Lambda([this, Reaction, GainXPReaction, Type](int32 NewValue, ETextCommit::Type)
					{
						GainXPReaction->XP = NewValue;
						OnEventReactionEdited(Reaction, Type);
					})
				]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventReactionWidget_Reputation(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type)
{
	FZEventReaction_Reputation* ReputationReaction = Reaction->GetMutablePtr<FZEventReaction_Reputation>();
	return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SEnumComboBox, StaticEnum<EZFaction>())
					.CurrentValue_Lambda([ReputationReaction]()
					{
						return static_cast<int32>(ReputationReaction->Faction);
					})
					.OnEnumSelectionChanged_Lambda([this, Reaction, ReputationReaction, Type](int32 NewValue, ESelectInfo::Type)
					{
						ReputationReaction->Faction = static_cast<EZFaction>(NewValue);
						OnEventReactionEdited(Reaction, Type);
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SNumericEntryBox<float>)
					.AllowSpin(false)
					.Value_Lambda([this, ReputationReaction]()
					{
						return TOptional<float>(ReputationReaction->Change);
					})
					.OnValueCommitted_Lambda([this, Reaction, ReputationReaction, Type](float NewValue, ETextCommit::Type)
					{
						ReputationReaction->Change = NewValue;
						OnEventReactionEdited(Reaction, Type);
					})
				]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventReactionWidget_HomeTransform(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type)
{
	FZEventReaction_HomeTransform* HomeTransformReaction = Reaction->GetMutablePtr<FZEventReaction_HomeTransform>();
	return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SZGameplayTagPickerButton)
					.RootTag(ZGameplayTags::Waypoint)
					.PreSelected(ZGameplayTags::Waypoint.GetTag().GetSingleTagContainer())
					.OnTagsChanged_Lambda([this, Reaction, HomeTransformReaction, Type](const FGameplayTagContainer& NewSelection)
					{
						HomeTransformReaction->WaypointTag = (NewSelection.Num() > 0) ? NewSelection.First() : FGameplayTag::EmptyTag;
						OnEventReactionEdited(Reaction, Type);
					})
				]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventReactionWidget_StartPath(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type)
{
	FZEventReaction_StartPath* StartPathReaction = Reaction->GetMutablePtr<FZEventReaction_StartPath>();
	return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SZGameplayTagPickerButton)
					.RootTag(ZGameplayTags::Path)
					.PreSelected(ZGameplayTags::Path.GetTag().GetSingleTagContainer())
					.OnTagsChanged_Lambda([this, Reaction, StartPathReaction, Type](const FGameplayTagContainer& NewSelection)
					{
						StartPathReaction->PathTag = (NewSelection.Num() > 0) ? NewSelection.First() : FGameplayTag::EmptyTag;
						OnEventReactionEdited(Reaction, Type);
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SCheckBox)
					.IsChecked_Lambda([StartPathReaction]()
					{
						return StartPathReaction->bEscortPlayer ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
					})
					.OnCheckStateChanged_Lambda([this, Reaction, StartPathReaction, Type](ECheckBoxState NewState)
					{
						StartPathReaction->bEscortPlayer = NewState == ECheckBoxState::Checked;
						OnEventReactionEdited(Reaction, Type);
					})
					[
						SNew(STextBlock)
						.Text(LOCTEXT("EscortPlayer", "Escort player"))
					]
				]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventReactionWidget_CancelPath(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type)
{
	FZEventReaction_CancelPath* CancelPathReaction = Reaction->GetMutablePtr<FZEventReaction_CancelPath>();
	return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SZGameplayTagPickerButton)
					.RootTag(ZGameplayTags::Path)
					.PreSelected(ZGameplayTags::Path.GetTag().GetSingleTagContainer())
					.OnTagsChanged_Lambda([this, Reaction, CancelPathReaction, Type](const FGameplayTagContainer& NewSelection)
					{
						CancelPathReaction->PathTag = (NewSelection.Num() > 0) ? NewSelection.First() : FGameplayTag::EmptyTag;
						OnEventReactionEdited(Reaction, Type);
					})
				]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventReactionWidget_StartTimedEvent(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type)
{
	FZEventReaction_StartTimedEvent* StartTimedEventReaction = Reaction->GetMutablePtr<FZEventReaction_StartTimedEvent>();
	return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SZGameplayTagPickerButton)
					.RootTag(ZGameplayTags::Event_Timed)
					.PreSelected(ZGameplayTags::Event_Timed.GetTag().GetSingleTagContainer())
					.OnTagsChanged_Lambda([this, Reaction, StartTimedEventReaction, Type](const FGameplayTagContainer& NewSelection)
					{
						StartTimedEventReaction->TimedEvent = (NewSelection.Num() > 0) ? NewSelection.First() : FGameplayTag::EmptyTag;
						OnEventReactionEdited(Reaction, Type);
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SNumericEntryBox<int32>)
					.AllowSpin(false)
					.Value_Lambda([this, StartTimedEventReaction]()
					{
						return TOptional<int32>(StartTimedEventReaction->DurationHours);
					})
					.OnValueCommitted_Lambda([this, Reaction, StartTimedEventReaction, Type](int32 NewValue, ETextCommit::Type)
					{
						StartTimedEventReaction->DurationHours = NewValue;
						OnEventReactionEdited(Reaction, Type);
					})
				]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventReactionWidget_SetCanTrade(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type)
{
	FZEventReaction_SetCanTrade* SetCanTradeEventReaction = Reaction->GetMutablePtr<FZEventReaction_SetCanTrade>();
	return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SCheckBox)
					.IsChecked_Lambda([SetCanTradeEventReaction]()
					{
						return SetCanTradeEventReaction->bCanTrade ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
					})
					.OnCheckStateChanged_Lambda([this, Reaction, SetCanTradeEventReaction, Type](ECheckBoxState NewState)
					{
						SetCanTradeEventReaction->bCanTrade = NewState == ECheckBoxState::Checked;
						OnEventReactionEdited(Reaction, Type);
					})
					[
						SNew(STextBlock)
						.Text(LOCTEXT("CanTrade", "CanTrade"))
					]
				]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventReactionsWidget(EZEventReactionType Type)
{
	TArray<TSharedPtr<FInstancedStruct>>& EventReactionList = GetEventReactionList(Type);
	EventReactionList.Reset();
	for (const TInstancedStruct<FZEventReaction>& Reaction : GetEditedEventReactionList(Type))
	{
		EventReactionList.Add(MakeShared<FInstancedStruct>(Reaction));
	}
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			bEditMode ? 
				SNew(SButton)
				.Text(LOCTEXT("AddNewReaction", "Add new reaction"))
				.OnClicked_Lambda([this, Type]()
				{
					FMenuBuilder MenuBuilder(true, nullptr);
					MenuBuilder.AddWidget(CreateReactionTypeVBox(Type)
						, FText::GetEmpty());
				
					ShowPopupMenu(MenuBuilder);
					return FReply::Handled();
				})
				: SNullWidget::NullWidget
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(GetEventReactionListView(Type), SListView<TSharedPtr<FInstancedStruct>>)
			.ListItemsSource(&EventReactionList)
			.SelectionMode(ESelectionMode::Type::None)
			.OnGenerateRow_Lambda([this, Type](TSharedPtr<FInstancedStruct> Item, const TSharedRef<STableViewBase>& OwnerTable)
			{
				TSharedRef<SWidget> Row = SNullWidget::NullWidget;
				FText Text = FText::GetEmpty();
				if (bEditMode)
				{
					for (const FZEventReactionData& Reaction : ReactionData)
					{
						if (Item->GetScriptStruct() == Reaction.Type)
						{
							if (Reaction.BuildEditableWidgetFunction.IsSet())
							{
								Row = Reaction.BuildEditableWidgetFunction(Item, Type);
							}
							Text = Reaction.Text;
						}
					}
				}
				else
				{
					Row = SNew(STextBlock)
							.Text(FText::FromString(Item.Get()->Get<FZEventReaction>().ToString()));
				}
				return SNew(STableRow<TSharedPtr<FGameplayTag>>, OwnerTable)
						[
							SNew(SBorder)
							.BorderImage(FAppStyle::GetBrush("Border"))
							.BorderBackgroundColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.f))
							.Padding(2.f)
							[
								SNew(SVerticalBox)
								+SVerticalBox::Slot()
								.AutoHeight()
								[
									bEditMode
									? SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.FillWidth(1.f)
										[
											SNew(STextBlock)
												.Text(FText::Format(INVTEXT("{0}:"), Text))
										]
										+ SHorizontalBox::Slot()
										.AutoWidth()
										.VAlign(VAlign_Top)
										[
											BuildRemoveReactionButton(Item, Type)
										]
									: SNullWidget::NullWidget
								]
								+SVerticalBox::Slot()
								.AutoHeight()
								[
									Row
								]
							]
						];
			})
		];
}

TArray<TSharedPtr<FInstancedStruct>>& SZEventEditor::GetEventReactionList(EZEventReactionType Type)
{
	switch (Type) {
	case EZEventReactionType::OnAvailable:
		return EventOnAvailableReactionList;
	case EZEventReactionType::OnUnavailable:
		return EventOnUnavailableReactionList;
	default:
		check(0);
	case EZEventReactionType::OnCompleted:
		return EventOnCompletedReactionList;
	}
}

TArray<TInstancedStruct<FZEventReaction>>& SZEventEditor::GetEditedEventReactionList(EZEventReactionType Type)
{
	switch (Type) {
	case EZEventReactionType::OnAvailable:
		return EditedEvent.OnAvailable;
	case EZEventReactionType::OnUnavailable:
		return EditedEvent.OnUnavailable;
	default:
		check(0);
	case EZEventReactionType::OnCompleted:
		return EditedEvent.OnCompleted;
	}
}

TSharedPtr<SListView<TSharedPtr<FInstancedStruct>>>& SZEventEditor::GetEventReactionListView(EZEventReactionType Type)
{
	switch (Type) {
	case EZEventReactionType::OnAvailable:
		return EventOnAvailableReactionListView;
	case EZEventReactionType::OnUnavailable:
		return EventOnUnavailableReactionListView;
	default:
		check(0);
	case EZEventReactionType::OnCompleted:
		return EventOnCompletedReactionListView;
	}
}

TSharedRef<SWidget> SZEventEditor::BuildRemoveConditionButton(TSharedPtr<FInstancedStruct> Condition)
{
	return SNew(SBox)
			.Padding(6.f, 0.f, 0.f, 0.f)
			[
				SNew(SButton)
				.OnClicked_Lambda([this, Condition]()
				{
					const int32 Index = EventConditionList.IndexOfByKey(Condition);
					if (ensure(Index != INDEX_NONE))
					{
						EventConditionList.RemoveAt(Index);
						EditedDialogueOption.Conditions.RemoveAt(Index);
						OnDialogueEdited();
						EventConditionListView->RequestListRefresh();
					}
					return FReply::Handled();
				})
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("GenericCommands.Delete"))
				]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventReputationConditionWidget(TSharedPtr<FInstancedStruct> Condition)
{
	FZEventCondition_Reputation* ReputationCondition = Condition->GetMutablePtr<FZEventCondition_Reputation>();
	return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SEnumComboBox, StaticEnum<EZFaction>())
					.CurrentValue_Lambda([ReputationCondition]()
					{
						return static_cast<int32>(ReputationCondition->Faction);
					})
					.OnEnumSelectionChanged_Lambda([this, Condition, ReputationCondition](int32 NewValue, ESelectInfo::Type)
					{
						ReputationCondition->Faction = static_cast<EZFaction>(NewValue);
						OnDialogueConditionEdited(Condition);
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SCheckBox)
					.IsChecked_Lambda([ReputationCondition]()
					{
						return ReputationCondition->bOver ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
					})
					.OnCheckStateChanged_Lambda([this, Condition, ReputationCondition](ECheckBoxState NewState)
					{
						ReputationCondition->bOver = NewState == ECheckBoxState::Checked;
						OnDialogueConditionEdited(Condition);
					})
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Over", "Over"))
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SNumericEntryBox<float>)
					.AllowSpin(false)
					.Value_Lambda([this, ReputationCondition]()
					{
						return TOptional<float>(ReputationCondition->Threshold);
					})
					.OnValueCommitted_Lambda([this, Condition, ReputationCondition](float NewValue, ETextCommit::Type)
					{
						ReputationCondition->Threshold = NewValue;
						OnDialogueConditionEdited(Condition);
					})
				]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventInventoryConditionWidget(TSharedPtr<FInstancedStruct> Condition)
{
	FZEventCondition_Inventory* InventoryCondition = Condition->GetMutablePtr<FZEventCondition_Inventory>();
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(UZInventoryItemData::StaticClass())
				.ObjectPath_Lambda([InventoryCondition]()
				{
					FAssetData AssetData;
					UAssetManager::Get().GetPrimaryAssetData(InventoryCondition->ItemID, AssetData);
					return AssetData.GetObjectPathString();
				})
				.OnObjectChanged_Lambda([this, Condition, InventoryCondition](const FAssetData& AssetData)
				{
					InventoryCondition->ItemID = AssetData.GetPrimaryAssetId();
					OnDialogueConditionEdited(Condition);
				})
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SNumericEntryBox<int32>)
				.AllowSpin(false)
				.Value_Lambda([this, InventoryCondition]()
				{
					return TOptional<int32>(InventoryCondition->Count);
				})
				.OnValueCommitted_Lambda([this, Condition, InventoryCondition](int32 NewValue, ETextCommit::Type)
				{
					InventoryCondition->Count = NewValue;
					OnDialogueConditionEdited(Condition);
				})
			]
		];
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventSkillConditionWidget(TSharedPtr<FInstancedStruct> Condition)
{
	FZEventCondition_Skill* SkillCondition = Condition->GetMutablePtr<FZEventCondition_Skill>();
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SEnumComboBox, StaticEnum<EZSkill>())
				.CurrentValue_Lambda([SkillCondition]()
				{
					return static_cast<int32>(SkillCondition->RequiredSkill);
				})
				.OnEnumSelectionChanged_Lambda([this, SkillCondition, Condition](int32 NewValue, ESelectInfo::Type)
				{
					SkillCondition->RequiredSkill = static_cast<EZSkill>(NewValue);
					OnDialogueConditionEdited(Condition);
				})
			]
		];
}

TSharedRef<SWidget> SZEventEditor::CreateConditionTypeVBox()
{
	TSharedRef<SVerticalBox> VBox = SNew(SVerticalBox);
	for (const FZEventConditionData& Condition : ConditionData)
	{
		VBox->AddSlot()
		.AutoHeight()
		[
			SNew(SButton)
			.OnClicked_Lambda([this, Condition]()
			{
				TSharedPtr<FInstancedStruct> NewCondition = MakeShared<FInstancedStruct>();
				NewCondition->InitializeAs(Condition.Type);
				EventConditionList.Add(NewCondition);
				EditedDialogueOption.Conditions.AddDefaulted();
				OnDialogueConditionEdited(NewCondition);
                
				EventConditionListView->RequestListRefresh();
				HidePopupMenu();
				
				return FReply::Handled();
			})
			[
				SNew(STextBlock).Text(Condition.Text)
			]
		];
	}
	return VBox;
}

TSharedRef<SWidget> SZEventEditor::BuildEditableEventConditionsWidget()
{
	EventConditionList.Reset();
	for (const TInstancedStruct<FZEventCondition>& Condition : EditedDialogueOption.Conditions)
	{
		EventConditionList.Add(MakeShared<FInstancedStruct>(Condition));
	}
	return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				bEditMode ? 
					SNew(SButton)
					.Text(LOCTEXT("AddNewCondition", "Add new condition"))
					.OnClicked_Lambda([this]()
					{
						FMenuBuilder MenuBuilder(true, nullptr);
						MenuBuilder.AddWidget(CreateConditionTypeVBox(), FText::GetEmpty());
					
						ShowPopupMenu(MenuBuilder);
						return FReply::Handled();
					})
					: SNullWidget::NullWidget
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(EventConditionListView, SListView<TSharedPtr<FInstancedStruct>>)
				.ListItemsSource(&EventConditionList)
				.SelectionMode(ESelectionMode::Type::None)
				.OnGenerateRow_Lambda([this](TSharedPtr<FInstancedStruct> Item, const TSharedRef<STableViewBase>& OwnerTable)
				{
					TSharedRef<SWidget> Row = SNullWidget::NullWidget;
					FText Text = FText::GetEmpty();
					if (bEditMode)
					{
						for (const FZEventConditionData& Condition : ConditionData)
						{
							if (Item->GetScriptStruct() == Condition.Type)
							{
								if (Condition.BuildEditableWidgetFunction.IsSet())
								{
									Row = Condition.BuildEditableWidgetFunction(Item);
								}
								Text = Condition.Text;
								break;
							}
						}
					}
					else
					{
						Row = SNew(STextBlock)
								.Text(FText::FromString(Item.Get()->Get<FZEventCondition>().ToString()));
					}
					return SNew(STableRow<TSharedPtr<FGameplayTag>>, OwnerTable)
							[
								SNew(SBorder)
								.BorderImage(FAppStyle::GetBrush("Border"))
								.BorderBackgroundColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.f))
								.Padding(2.f)
								[
									SNew(SVerticalBox)
									+SVerticalBox::Slot()
									.AutoHeight()
									[
										bEditMode
										? SNew(SHorizontalBox)
											+ SHorizontalBox::Slot()
											.FillWidth(1.f)
											[
												SNew(STextBlock)
												.Text(FText::Format(INVTEXT("{0}:"), Text))
											]
											+ SHorizontalBox::Slot()
											.AutoWidth()
											.VAlign(VAlign_Top)
											[
												BuildRemoveConditionButton(Item)
											]
										: SNullWidget::NullWidget
									]
									+SVerticalBox::Slot()
									.AutoHeight()
									[
										Row
									]
								]
							];
				})
			];
}

TSharedRef<SWidget> SZEventEditor::BuildDialogueView()
{
	if (!CurrentEvent.IsValid() || GetEventType(CurrentEvent) != EZEventType::Dialogue)
	{
		return SNullWidget::NullWidget;
	}
	EditedDialogueOption = *DialogueOptionsMap[CurrentEvent];

	TSharedPtr<FGameplayTagNode> NPCDialogueNode = GetNextNode(CurrentEvent, ZGameplayTags::Event_Dialogue);
	const FGameplayTag ContinueFromParentTag = NPCDialogueNode.IsValid() ? NPCDialogueNode->GetCompleteTag() : ZGameplayTags::Event_Dialogue;

	constexpr FLinearColor BorderOutline = FLinearColor(0.5f, 0.5f, 0.5f, 1.f);
	constexpr float Padding = 2.f;
	return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Border"))
					.BorderBackgroundColor(BorderOutline)
					.Padding(Padding)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							BuildDetailsPropertyNameWidget(LOCTEXT("DialogueText", "Dialogue Text:"))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.f)
						[
							BuildEditableTextWidget(EditedDialogueOption.Text, EZEditEventType::Dialogue)
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Border"))
					.BorderBackgroundColor(BorderOutline)
					.Padding(Padding)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							BuildDetailsPropertyNameWidget(LOCTEXT("DialogueResponse", "Dialogue Response:"))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.f)
						[
							BuildEditableTextWidget(EditedDialogueOption.Response, EZEditEventType::Dialogue)
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Border"))
					.BorderBackgroundColor(BorderOutline)
					.Padding(Padding)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							BuildDetailsPropertyNameWidget(LOCTEXT("LocksDialogue", "Locks dialogue:"))
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							BuildEditableCheckboxWidget(EditedDialogueOption.bLocksDialogue, EZEditEventType::Dialogue)
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Border"))
					.BorderBackgroundColor(BorderOutline)
					.Padding(Padding)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							BuildDetailsPropertyNameWidget(LOCTEXT("ContinuationFrom", "Continues from:"))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.f)
						[
							BuildEditableGameplayTagWidget(EditedDialogueOption.ContinuationFrom, ContinueFromParentTag, EZEditEventType::Dialogue)
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Border"))
					.BorderBackgroundColor(BorderOutline)
					.Padding(Padding)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							BuildDetailsPropertyNameWidget(LOCTEXT("EndsDialogue", "Ends dialogue:"))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.f)
						[
							BuildEditableCheckboxWidget(EditedDialogueOption.bEndsDialogue, EZEditEventType::Dialogue)
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Border"))
					.BorderBackgroundColor(BorderOutline)
					.Padding(Padding)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							BuildDetailsPropertyNameWidget(LOCTEXT("DialogueContext", "Dialogue context:"))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.f)
						[
							BuildEditableEnumWidget(EditedDialogueOption.DialogueContext, EZEditEventType::Dialogue)
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Border"))
					.BorderBackgroundColor(BorderOutline)
					.Padding(Padding)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							BuildDetailsPropertyNameWidget(LOCTEXT("Repeatable", "Repeatable:"))
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							BuildCheckboxWidget(EditedDialogueOption.bRepeatable)
						]
					]
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.f)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Border"))
					.BorderBackgroundColor(BorderOutline)
					.Padding(Padding)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							BuildDetailsPropertyNameWidget(LOCTEXT("Conditions", "Conditions:"))
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							BuildEditableEventConditionsWidget()
						]
					]
				]
			]
			;
}

TSharedRef<SWidget> SZEventEditor::BuildEventChain(const TArray<FGameplayTag>& Tags, bool bBlocking) const
{
	TSharedRef<SHorizontalBox> Box = SNew(SHorizontalBox);
	for (int32 Idx = 0; Idx < Tags.Num(); Idx++)
	{
		const FText Text = FText::FromName(bShowPreviewFullNames ? Tags[Idx].GetTagName() : Tags[Idx].GetTagLeafName());
		Box->AddSlot()
		   .AutoWidth()
		[
			SNew(STextBlock)
			.Text(Text)
			.ColorAndOpacity((bBlocking && (Idx == 0)) ? FLinearColor::Red : FSlateColor::UseForeground())
		];
		if (Idx < Tags.Num() - 1)
		{
			Box->AddSlot()
			   .AutoWidth()
			[
				SNew(STextBlock)
				.Text(INVTEXT(" -> "))
			];
		}
	}
	return Box;
}

TArray<TArray<FGameplayTag>> SZEventEditor::CreateRequiredDependencyChains(FGameplayTag From, FGameplayTag To) const
{
	TArray<TArray<FGameplayTag>> DependencyChains;
	if (From == To)
	{
		ensure(0);
		return DependencyChains;
	}
	if (const FGameplayTagContainer* FromRequiredEvents = RelatedEvents[EZEventRelation::Required].Find(From))
	{
		for (const FGameplayTag FromRequiredEvent : *FromRequiredEvents)
		{
			if (FromRequiredEvent == To)
			{
				DependencyChains.Add({To, From});
			}
			else
			{
				for (TArray<FGameplayTag>& InnerDependencyChain : CreateRequiredDependencyChains(From, FromRequiredEvent, To))
				{
					if (ensure(!InnerDependencyChain.IsEmpty() && InnerDependencyChain[0] == To))
					{
						InnerDependencyChain.Add(From);
						DependencyChains.Add(InnerDependencyChain);
					}
				}
			}
		}
	}
	return DependencyChains;
}

TArray<TArray<FGameplayTag>> SZEventEditor::CreateRequiredDependencyChains(FGameplayTag From, FGameplayTag Current, FGameplayTag To) const
{
	TArray<TArray<FGameplayTag>> DependencyChains;
	if ((From == Current) || (Current == To))
	{
		ensure(0);
		return DependencyChains;
	}
	if (const FGameplayTagContainer* FromRequiredEvents = RelatedEvents[EZEventRelation::Required].Find(Current))
	{
		for (const FGameplayTag FromRequiredEvent : *FromRequiredEvents)
		{
			if (FromRequiredEvent == To)
			{
				DependencyChains.Add({To, Current});
			}
			else
			{
				for (TArray<FGameplayTag>& InnerDependencyChain : CreateRequiredDependencyChains(From, FromRequiredEvent, To))
				{
					if (ensure(!InnerDependencyChain.IsEmpty() && InnerDependencyChain[0] == To))
					{
						InnerDependencyChain.Add(Current);
						DependencyChains.Add(InnerDependencyChain);
					}
				}
			}
		}
	}
	return DependencyChains;
}

TArray<TArray<FGameplayTag>> SZEventEditor::CreateBlockedDependencyChains(FGameplayTag From, FGameplayTag To) const
{
	TArray<TArray<FGameplayTag>> DependencyChains;
	if (From == To)
	{
		ensure(0);
		return DependencyChains;
	}
	if (const FGameplayTagContainer* FromBlockedEvents = RelatedEvents[EZEventRelation::Blocked].Find(From))
	{
		for (const FGameplayTag FromBlockedEvent : *FromBlockedEvents)
		{
			if (FromBlockedEvent == To)
			{
				DependencyChains.Add({From, To});
			}
			else
			{
				for (TArray<FGameplayTag>& InnerDependencyChain : CreateRequiredDependencyChains(To, FromBlockedEvent))
				{
					InnerDependencyChain.Insert(From, 0);
					DependencyChains.Add(InnerDependencyChain);
				}
			}
		}
	}
	return DependencyChains;
}

TSharedRef<SWidget> SZEventEditor::BuildPreview(FGameplayTag Target)
{
	TSharedRef<SVerticalBox> Box = SNew(SVerticalBox);
	if (CurrentDirectlyRelatedEvents[EZEventRelation::Required].HasTagExact(Target) || CurrentIndirectlyRelatedEvents[EZEventRelation::Required].HasTagExact(Target))
	{
		for (const TArray<FGameplayTag>& Chain : CreateRequiredDependencyChains(CurrentEvent, Target))
		{
			Box->AddSlot()
			   .AutoHeight()
			[
				BuildEventChain(Chain, false)
			];
		}
	}
	if (CurrentDirectlyRelatedEvents[EZEventRelation::Blocked].HasTagExact(Target) || CurrentIndirectlyRelatedEvents[EZEventRelation::Blocked].HasTagExact(Target))
	{
		for (const TArray<FGameplayTag>& Chain : CreateBlockedDependencyChains(CurrentEvent, Target))
		{
			Box->AddSlot()
			   .AutoHeight()
			[
				BuildEventChain(Chain, true)
			];
		}
	}
	if (CurrentDirectlyRelatedEvents[EZEventRelation::RequiredBy].HasTagExact(Target) || CurrentIndirectlyRelatedEvents[EZEventRelation::RequiredBy].HasTagExact(Target))
	{
		for (const TArray<FGameplayTag>& Chain : CreateRequiredDependencyChains(Target, CurrentEvent))
		{
			Box->AddSlot()
			   .AutoHeight()
			[
				BuildEventChain(Chain, false)
			];
		}
	}
	if (CurrentDirectlyRelatedEvents[EZEventRelation::BlockedBy].HasTagExact(Target) || CurrentIndirectlyRelatedEvents[EZEventRelation::BlockedBy].HasTagExact(Target))
	{
		for (const TArray<FGameplayTag>& Chain : CreateBlockedDependencyChains(Target, CurrentEvent))
		{
			Box->AddSlot()
			   .AutoHeight()
			[
				BuildEventChain(Chain, true)
			];
		}
	}

	return SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			Box
		];
}

void SZEventEditor::UpdateNewEventWarning()
{
	FTextBuilder Builder;
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	TSharedPtr<FGameplayTagNode> TagNode = TagsManager.FindTagNode(NewEventRootTag);
	if (!TagNode.IsValid())
	{
		Builder.AppendLine(LOCTEXT("InvalidRootTag", "Invalid root tag."));
	}
	else
	{
		if (TagNode->GetChildTagNodes().ContainsByPredicate([this](const TSharedPtr<FGameplayTagNode>& ChildNode)
		{
			return ChildNode->GetSimpleTagName().ToString() == NewEvent.TagName.ToString();
		}))
		if (EventTypes[EZEventType::Regular].Events.HasTagExact(NewEvent.RootTag) || EventTypes[EZEventType::Dialogue].Events.HasTagExact(NewEvent.RootTag))
		{
			Builder.AppendLine(LOCTEXT("EventAlreadyAdded", "Selected event has already been added to a data table."));
		}
	}
	InsertWidgetWarningText->SetText(Builder.ToText());
}

TSharedRef<SWidget> SZEventEditor::BuildEventTypeComboBox()
{
	TSharedPtr<EZEventType> InitiallySelectedItem = nullptr;
	if (ensure(FilteredEventTypes.Num() > 0))
	{
		const int32 Idx = FilteredEventTypes.IndexOfByPredicate([this](TSharedPtr<EZEventType> EventType)
		{
			return NewEvent.Type == *EventType;
		});
		if (ensure(Idx != INDEX_NONE))
		{
			InitiallySelectedItem = FilteredEventTypes[Idx];
		}
	}
	return SAssignNew(EventTypeComboBox, SComboBox<TSharedPtr<EZEventType>>)
			.OptionsSource(&FilteredEventTypes)
			.InitiallySelectedItem(InitiallySelectedItem)
			.OnGenerateWidget_Lambda([this](TSharedPtr<EZEventType> Item)
			{
				if (!Item.IsValid())
				{
					return SNew(STextBlock)
							.Text(ZEditorText::Invalid);
				}
				return SNew(STextBlock)
						.Text(GetEventTypeText(*Item));
			})
			.OnSelectionChanged_Lambda([this](TSharedPtr<EZEventType> Item, ESelectInfo::Type)
			{
				if (Item.IsValid())
				{
					NewEvent.Type = *Item;
				}
				else
				{
					NewEvent.Type = EZEventType::Simple;
				}
				RefreshFilteredDataTables();
			})
			[
				SNew(STextBlock)
				.Text_Lambda([this]()
				{
					return GetEventTypeText(NewEvent.Type);
				})
			];
}

void SZEventEditor::RefreshFilteredEventTypes()
{
	const FGameplayTag SelectedTag = NewEvent.RootTag;
	FilteredEventTypes.Empty();
	if (SelectedTag.MatchesTag(ZGameplayTags::Event_Dialogue))
	{
		FilteredEventTypes.Add(MakeShared<EZEventType>(EZEventType::Dialogue));
	}
	else if (SelectedTag.MatchesTag(ZGameplayTags::Event_Timed))
	{
		FilteredEventTypes.Add(MakeShared<EZEventType>(EZEventType::Timed));
	}
	else
	{
		FilteredEventTypes.Add(MakeShared<EZEventType>(EZEventType::Regular));
		FilteredEventTypes.Add(MakeShared<EZEventType>(EZEventType::Simple));
	}
	// If current selection isn't valid select the first valid one
	if (ensure(!FilteredEventTypes.IsEmpty()))
	{
		if (!FilteredEventTypes.ContainsByPredicate([this](TSharedPtr<EZEventType> Type)
		{
			return NewEvent.Type == *Type;
		}))
		{
			NewEvent.Type = *FilteredEventTypes[0];
		}
	}
	// Set selection to the type of new event
	if (EventTypeComboBox.IsValid())
	{
		EventTypeComboBox->RefreshOptions();
		if (TSharedPtr<EZEventType>* FoundOption = FilteredEventTypes.FindByPredicate([this](const TSharedPtr<EZEventType>& EventType)
			{
				return NewEvent.Type == *EventType;
			}))
		{
			EventTypeComboBox->SetSelectedItem(*FoundOption);
		}
		else
		{
			ensure(0);
		}
	}
	RefreshFilteredDataTables();
}

TSharedRef<SWidget> SZEventEditor::BuildDataTableComboBox()
{
	TWeakObjectPtr<UDataTable> InitiallySelectedItem = nullptr;
	const bool bHasDataTables = FilteredDataTables.Num() > 0;
	if (bHasDataTables)
	{
		const int32 Idx = FilteredDataTables.IndexOfByPredicate([this](TWeakObjectPtr<UDataTable> DataTable)
		{
			return NewEvent.DataTable == DataTable.Get();
		});
		if (ensure(Idx != INDEX_NONE))
		{
			InitiallySelectedItem = FilteredDataTables[Idx];
		}
	}
	return SAssignNew(DataTableComboBox, SComboBox<TWeakObjectPtr<UDataTable>>)
			.OptionsSource(&FilteredDataTables)
			.InitiallySelectedItem(InitiallySelectedItem)
			.IsEnabled_Lambda([this]()
			{
				return (FilteredDataTables.Num() > 0);
			})
			.OnGenerateWidget_Lambda([this](TWeakObjectPtr<UDataTable> Item)
			{
				if (!Item.IsValid())
				{
					return SNew(STextBlock)
							.Text(ZEditorText::Invalid);
				}
				return SNew(STextBlock)
						.Text(FText::FromString(Item->GetName()));
			})
			.OnSelectionChanged_Lambda([this](TWeakObjectPtr<UDataTable> Item, ESelectInfo::Type)
			{
				NewEvent.DataTable = Item.Get();
			})
			[
				SNew(STextBlock)
				.Text_Lambda([this]()
				{
					return NewEvent.DataTable ? FText::FromString(NewEvent.DataTable.GetName()) : ZEditorText::None;
				})
			];
}

void SZEventEditor::RefreshFilteredDataTables()
{
	FilteredDataTables.Empty();
	for (const TPair<TObjectPtr<UDataTable>, FGameplayTagContainer>& Pair : EventTypes[NewEvent.Type].EventTables)
	{
		FilteredDataTables.Add(TWeakObjectPtr<UDataTable>(Pair.Key));
	}
	// If current selection isn't valid select the first valid one
	if (FilteredDataTables.IsEmpty())
	{
		NewEvent.DataTable = nullptr;
	}
	else
	{
		if (!FilteredDataTables.ContainsByPredicate([this](TWeakObjectPtr<UDataTable> DataTable)
		{
			return NewEvent.DataTable == DataTable.Get();
		}))
		{
			NewEvent.DataTable = FilteredDataTables[0].Get();
		}
	}
	// Set selection to the type of new event
	if (DataTableComboBox.IsValid())
	{
		DataTableComboBox->RefreshOptions();
		if (TWeakObjectPtr<UDataTable>* FoundOption = FilteredDataTables.FindByPredicate([this](TWeakObjectPtr<UDataTable> DataTable)
			{
				return NewEvent.DataTable == DataTable.Get();
			}))
		{
			DataTableComboBox->SetSelectedItem(*FoundOption);
		}
		else
		{
			DataTableComboBox->ClearSelection();
		}
	}
}

TSharedRef<SWidget> SZEventEditor::BuildInsertEventTagWidget()
{
	return SNew(SBox)
			.WidthOverride(400.f)
			[
				SNew(SBorder)
				.Padding(FMargin(8.f))
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(ZEditorText::RootTag)
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.f)
						[
							SNew(SZGameplayTagPickerButton)
							.RootTag(ZGameplayTags::Event)
							.OnTagsChanged_Lambda([this](const FGameplayTagContainer& SelectedTag)
							{
								NewEventRootTag = SelectedTag.Num() > 0 ? SelectedTag.First() : ZGameplayTags::Event;
							})
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("NewTagName", "New tag name"))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.f)
						[
							SNew(SEditableTextBox)
							.OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type)
							{
								NewEventText = NewText;
							})
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("NewTagComment", "New tag comment"))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.f)
						[
							SNew(SEditableTextBox)
							.OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type)
							{
								NewEventComment = NewText;
							})
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SButton)
						.Text(LOCTEXT("InsertNewTag", "Insert new tag"))
						.OnClicked_Lambda([this]()
						{
							const FString NewTag = FString::Printf(TEXT("%s.%s"), *NewEventRootTag.ToString(), *NewEventText.ToString());
							if (IGameplayTagsEditorModule::Get().AddNewGameplayTagToINI(NewTag, NewEventComment.ToString()))
							{
								const FGameplayTag NewGameplayTag = UGameplayTagsManager::Get().RequestGameplayTag(FName(*NewTag));
								if (NewGameplayTag.IsValid())
								{
									EventTypes[EZEventType::Simple].Events.AddTag(NewGameplayTag);
									RefreshLists();
								}
								else
								{
									UE_LOG(LogZEventEditor, Error, TEXT("Error retrieving new tag: %s"), *NewTag);
								}
							}
							return FReply::Handled();
						})
					]
				]
			];
}

TSharedRef<SWidget> SZEventEditor::BuildInsertEventWidget()
{
	NewEvent.RootTag = CurrentEvent;
	RefreshFilteredEventTypes();

	FGameplayTagContainer UnusedEvents = UGameplayTagsManager::Get().RequestGameplayTagChildren(ZGameplayTags::Event);
	// UnusedEvents.RemoveTags(AllRegularEvents);
	// UnusedEvents.RemoveTags(AllDialogueOptions);
	
	constexpr float NameColumnWidth = 100.f;
	return SNew(SBox)
		.MinDesiredWidth(500.f)
		[
			SNew(SBorder)
			.Padding(FMargin(8.f))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SBox)
						.MinDesiredWidth(NameColumnWidth)
						[
							SNew(STextBlock)
							.Text(ZEditorText::RootTag)
						]
					]
					+ SHorizontalBox::Slot()
					.FillWidth(1.f)
					[
						SNew(SZGameplayTagPickerButton)
						.RootTag(ZGameplayTags::Event)
						.PreSelected(NewEvent.RootTag.GetSingleTagContainer())
						//.FilteredTags(UnusedEvents)
						.OnTagsChanged_Lambda([this](const FGameplayTagContainer& NewTag)
						{
							NewEvent.RootTag = (NewTag.Num() > 0) ? NewTag.First() : FGameplayTag::EmptyTag;
							RefreshFilteredEventTypes();
							UpdateNewEventWarning();
							
							GEditor->GetTimerManager()->SetTimerForNextTick([this]()
							{
								FSlateApplication::Get().SetKeyboardFocus(NewEventNameBox, EFocusCause::SetDirectly);
							});
						})
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SBox)
						.MinDesiredWidth(NameColumnWidth)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("NewTag", "New tag"))
						]
					]
					+ SHorizontalBox::Slot()
					.FillWidth(1.f)
					[
						SAssignNew(NewEventNameBox, SEditableTextBox)
						.OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type Type)
							{
								NewEvent.TagName = NewText;
								UpdateNewEventWarning();
							
								if (Type == ETextCommit::Type::OnEnter)
								{
									InsertEvent();
								}
							})
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SBox)
						.MinDesiredWidth(NameColumnWidth)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("EventType", "Event type"))
						]
					]
					+ SHorizontalBox::Slot()
					.FillWidth(1.f)
					[
						BuildEventTypeComboBox()
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SBox)
						.MinDesiredWidth(NameColumnWidth)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("DataTable", "Data table"))
						]
					]
					+ SHorizontalBox::Slot()
					.FillWidth(1.f)
					[
						BuildDataTableComboBox()
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(InsertWidgetWarningText, STextBlock)
					.Text(FText::GetEmpty())
					.ColorAndOpacity(FLinearColor::Red)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SButton)
					.Text(ZEditorText::Add)
					.OnClicked(this, &SZEventEditor::InsertEvent)
				]
			]
		];
}

void SZEventEditor::LoadRegularEvent(FZEventRow* EventRow, EZEventType Type)
{
	if (Type == EZEventType::Regular)
	{
		EventTypes[EZEventType::Regular].Events.AddTag(EventRow->ID);
		EventsMap.Add(EventRow->ID, EventRow);
	}
	else if (Type == EZEventType::Dialogue)
	{
		EventTypes[EZEventType::Dialogue].Events.AddTag(EventRow->ID);
		DialogueOptionsMap.Add(EventRow->ID, static_cast<FZDialogueOptionRow*>(EventRow));
	}
	
	if ((Type == EZEventType::Regular) || (Type == EZEventType::Dialogue))
	{
		RelatedEvents[EZEventRelation::Required].Add(EventRow->ID, EventRow->RequiredEvents);
		RelatedEvents[EZEventRelation::BlockedBy].Add(EventRow->ID, EventRow->BlockedByEvents);
		for (const FGameplayTag RequiredEvent : EventRow->RequiredEvents)
		{
			RelatedEvents[EZEventRelation::RequiredBy].FindOrAdd(RequiredEvent).AddTag(EventRow->ID);
		}
		for (const FGameplayTag BlockedByEvent : EventRow->BlockedByEvents)
		{
			RelatedEvents[EZEventRelation::Blocked].FindOrAdd(BlockedByEvent).AddTag(EventRow->ID);
		}
	}
}

void SZEventEditor::LoadEvents()
{
	for (int32 Idx = 0; Idx < static_cast<uint8>(EZEventType::MAX); ++Idx)
	{
		EventTypes[static_cast<EZEventType>(Idx)] = FZEventTypeData();
	}
	EventsMap.Empty();
	DialogueOptionsMap.Empty();
	
	RelatedEvents[EZEventRelation::Required].Empty();
	RelatedEvents[EZEventRelation::Blocked].Empty();
	RelatedEvents[EZEventRelation::RequiredBy].Empty();
	RelatedEvents[EZEventRelation::BlockedBy].Empty();
	NewEvent.Reset();

	const UZEventEditorSettings* Settings = GetDefault<UZEventEditorSettings>();
	for (FSoftObjectPath EventTablePath : Settings->GetEventsDataTables())
	{
		if (UDataTable* Table = Cast<UDataTable>(EventTablePath.TryLoad()))
		{
			FGameplayTagContainer AllTableEvents;
			TArray<FZEventRow*> AllRows;
			Table->GetAllRows(TEXT("SZEventEditor::LoadEvents"), AllRows);
			for (FZEventRow* EventRow : AllRows)
			{
				LoadRegularEvent(EventRow, EZEventType::Regular);
				AllTableEvents.AddTag(EventRow->ID);
			}
			EventTypes[EZEventType::Regular].EventTables.Emplace(Table, AllTableEvents);
		}
	}

	for (FSoftObjectPath DialogueTablePath : Settings->GetDialogueOptionsDataTables())
	{
		if (UDataTable* Table = Cast<UDataTable>(DialogueTablePath.TryLoad()))
		{
			FGameplayTagContainer AllTableEvents;
			TArray<FZDialogueOptionRow*> AllRows;
			Table->GetAllRows(TEXT("SZEventEditor::LoadEvents"), AllRows);
			for (FZDialogueOptionRow* DialogueOption : AllRows)
			{
				LoadRegularEvent(DialogueOption, EZEventType::Dialogue);
				AllTableEvents.AddTag(DialogueOption->ID);
			}
			EventTypes[EZEventType::Dialogue].EventTables.Emplace(Table, AllTableEvents);
		}
	}

	const UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	const FGameplayTag EventParentTag = FGameplayTag::RequestGameplayTag(FName("Event"));
	const FGameplayTagContainer AllEventTags = TagsManager.RequestGameplayTagChildren(EventParentTag);
	
	for (const FGameplayTag& EventTag : AllEventTags)
	{
		if (!EventTypes[EZEventType::Regular].Events.HasTagExact(EventTag) && !EventTypes[EZEventType::Dialogue].Events.HasTagExact(EventTag))
		{
			if (EventTag.MatchesTag(ZGameplayTags::Event_Timed))
			{
				const FName TagLeafName = EventTag.GetTagLeafName();
				if ((TagLeafName != ZGameplayTags::TimedEventCompletedLeafName) && (TagLeafName != ZGameplayTags::TimedEventExpiredLeafName))
				{
					EventTypes[EZEventType::Timed].Events.AddTag(EventTag);
				}
			}
			else
			{
				EventTypes[EZEventType::Simple].Events.AddTag(EventTag);
			}
		}
	}
	
	RefreshLists();
}

bool SZEventEditor::MatchesFilterWords(FGameplayTag EventTag, const TArray<FString> FilterWords) const
{
	const FString TagString = EventTag.ToString();
	for (const FString& FilterWord : FilterWords)
	{
		if (!TagString.Contains(FilterWord))
		{
			return false;
		}
	}
	return true;
}

void SZEventEditor::RefreshLists()
{
	const FString Filter = SearchBox.IsValid() ? SearchBox->GetText().ToString() : TEXT("");
	TArray<FString> FilterWords;
	Filter.ParseIntoArray(FilterWords, TEXT(" "), true);

	FilteredEvents.Empty();
	for (const FGameplayTag& EventTag : EventTypes[EZEventType::Regular].Events)
	{
		if (MatchesFilterWords(EventTag, FilterWords))
		{
			if (bShowRepeatable || !EventTag.MatchesTag(ZGameplayTags::Event_Repeatable))
			{
				FilteredEvents.Add(MakeShared<FGameplayTag>(EventTag));
			}
		}
	}
	// Sort alphabetically
	FilteredEvents.Sort([](const TSharedPtr<FGameplayTag>& A, const TSharedPtr<FGameplayTag>& B)
	{
		return A->GetTagName().LexicalLess(B->GetTagName());
	});
	if (EventListView.IsValid())
	{
		EventListView->RequestListRefresh();
	}

	FilteredDialogueOptions.Empty();
	for (const FGameplayTag& DialogueOptionTag : EventTypes[EZEventType::Dialogue].Events)
	{
		if (MatchesFilterWords(DialogueOptionTag, FilterWords))
		{
			if (bShowRepeatable || !DialogueOptionTag.MatchesTag(ZGameplayTags::Event_Repeatable))
			{
				FilteredDialogueOptions.Add(MakeShared<FGameplayTag>(DialogueOptionTag));
			}
		}
	}
	// Sort alphabetically
	FilteredDialogueOptions.Sort([](const TSharedPtr<FGameplayTag>& A, const TSharedPtr<FGameplayTag>& B)
	{
		return A->GetTagName().LexicalLess(B->GetTagName());
	});
	if (DialogueOptionListView.IsValid())
	{
		DialogueOptionListView->RequestListRefresh();
	}

	FilteredSimpleEvents.Empty();
	for (const FGameplayTag& SimpleEventTag : EventTypes[EZEventType::Simple].Events)
	{
		if (MatchesFilterWords(SimpleEventTag, FilterWords))
		{
			if (bShowRepeatable || !SimpleEventTag.MatchesTag(ZGameplayTags::Event_Repeatable))
			{
				FilteredSimpleEvents.Add(MakeShared<FGameplayTag>(SimpleEventTag));
			}
		}
	}
	// Sort alphabetically
	FilteredSimpleEvents.Sort([](const TSharedPtr<FGameplayTag>& A, const TSharedPtr<FGameplayTag>& B)
	{
		return A->GetTagName().LexicalLess(B->GetTagName());
	});
	if (SimpleEventListView.IsValid())
	{
		SimpleEventListView->RequestListRefresh();
	}

	FilteredTimedEvents.Empty();
	for (const FGameplayTag& TimedEventTag : EventTypes[EZEventType::Timed].Events)
	{
		if (MatchesFilterWords(TimedEventTag, FilterWords))
		{
			if (bShowRepeatable || !TimedEventTag.MatchesTag(ZGameplayTags::Event_Repeatable))
			{
				FilteredTimedEvents.Add(MakeShared<FGameplayTag>(TimedEventTag));
			}
		}
	}
	// Sort alphabetically
	FilteredTimedEvents.Sort([](const TSharedPtr<FGameplayTag>& A, const TSharedPtr<FGameplayTag>& B)
	{
		return A->GetTagName().LexicalLess(B->GetTagName());
	});
	if (TimedEventListView.IsValid())
	{
		TimedEventListView->RequestListRefresh();
	}
}

TSharedRef<ITableRow> SZEventEditor::OnGenerateEventRow(TSharedPtr<FGameplayTag> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	const FText Text = FText::FromName(Item->GetTagName());
	return SNew(STableRow<TSharedPtr<FGameplayTag>>, OwnerTable)
		[
			SNew(STextBlock)
			.Text(Text)
			.ToolTipText(Text)
			.Margin(FMargin(4.0f, 2.0f))
		];
}

void SZEventEditor::OnEventSelectedFromList(TSharedPtr<FGameplayTag> Item, ESelectInfo::Type SelectInfo)
{
	if (!Item.IsValid() || SelectInfo == ESelectInfo::Direct)
	{
		return;
	}
	if (Item.IsValid())
	{
		OnEventSelected(*Item);
	}
}

FReply SZEventEditor::OnEventSelected(FGameplayTag Item)
{
	NavigateTo(Item);
	return FReply::Handled();
}

FReply SZEventEditor::OnEventPreview(FGameplayTag Item)
{
	CurrentTarget = Item;
	PreviewArea->SetContent(BuildPreview(CurrentTarget));
	return FReply::Handled();
}

FReply SZEventEditor::OnCurrentEventRelationRemoved(EZEventRelation Relation, FGameplayTag SelectedEvent)
{
	FGameplayTagContainer& Container = CurrentDirectlyRelatedEvents[Relation];
	if (!Container.HasTagExact(SelectedEvent))
	{
		UE_LOG(LogZEventEditor, Error, TEXT("%s is already not related to %s (%s)"), *CurrentEvent.ToString(), *SelectedEvent.ToString(), *ENUM_TO_STRING(EZEventRelation, Relation));
		return FReply::Handled();
	}
	Container.RemoveTag(SelectedEvent);
	switch (Relation)
	{
	case EZEventRelation::Required:
		OnEventRelationRemoved(EZEventRelation::Required, CurrentEvent, SelectedEvent);
		break;
	case EZEventRelation::Blocked:
		OnEventRelationRemoved(EZEventRelation::Blocked, CurrentEvent, SelectedEvent);
		break;
	case EZEventRelation::RequiredBy:
		OnEventRelationRemoved(EZEventRelation::Required, SelectedEvent, CurrentEvent);
		break;
	case EZEventRelation::BlockedBy:
		OnEventRelationRemoved(EZEventRelation::Blocked, SelectedEvent, CurrentEvent);
		break;
	}
	RebuildContentArea();
	return FReply::Handled();
}

void SZEventEditor::OnEventRelationRemoved(EZEventRelation Relation, FGameplayTag From, FGameplayTag To)
{
	switch (Relation)
	{
	case EZEventRelation::Required:
		{
			FGameplayTagContainer* FromRequiredEvents = RelatedEvents[EZEventRelation::Required].Find(From);
			if (ensure(FromRequiredEvents && FromRequiredEvents->HasTagExact(To)))
			{
				FromRequiredEvents->RemoveTag(To);
			}
			FGameplayTagContainer* ToRequiredByEvents = RelatedEvents[EZEventRelation::RequiredBy].Find(To);
			if (ensure(ToRequiredByEvents && ToRequiredByEvents->HasTagExact(From)))
			{
				ToRequiredByEvents->RemoveTag(From);
			}
			break;
		}
	case EZEventRelation::Blocked:
		{
			FGameplayTagContainer* FromBlockedEvents = RelatedEvents[EZEventRelation::Blocked].Find(From);
			if (ensure(FromBlockedEvents && FromBlockedEvents->HasTagExact(To)))
			{
				FromBlockedEvents->RemoveTag(To);
			}
			FGameplayTagContainer* ToBlockedByEvents = RelatedEvents[EZEventRelation::BlockedBy].Find(To);
			if (ensure(ToBlockedByEvents && ToBlockedByEvents->HasTagExact(From)))
			{
				ToBlockedByEvents->RemoveTag(From);
			}
			break;
		}
	case EZEventRelation::RequiredBy:
		{
			ensure(0);
			break;
		}
	case EZEventRelation::BlockedBy:
		{
			ensure(0);
			break;
		}
	}
	OnEventEdited(From);
}

void SZEventEditor::EditCurrentEventRelation(EZEventRelation Relation)
{
	FMenuBuilder MenuBuilder(true, nullptr);
	MenuBuilder.AddWidget(
		SNew(SZGameplayTagPicker)
			.bMultiSelect(true)
			.PreSelected(CurrentDirectlyRelatedEvents[Relation])
			.PreExpanded(CurrentEvent.GetSingleTagContainer())
			.RootTag(ZGameplayTags::Event)
			.OnTagsChanged_Lambda([this, Relation](const FGameplayTagContainer& NewContainer)
			{
				FGameplayTagContainer Added = NewContainer;
				const FGameplayTagContainer& Container = CurrentDirectlyRelatedEvents[Relation];
				Added.RemoveTags(Container);
				for (const FGameplayTag Tag : Added)
				{
					OnCurrentEventRelationAdded(Relation, Tag);
				}
				FGameplayTagContainer Removed = Container;
				Removed.RemoveTags(NewContainer);
				for (const FGameplayTag Tag : Removed)
				{
					OnCurrentEventRelationRemoved(Relation, Tag);
				}
			})
			.OnClosed_Lambda([this]()
			{
				HidePopupMenu();
			})
		, FText::GetEmpty());

	ShowPopupMenu(MenuBuilder);
}

void SZEventEditor::OnCurrentEventRelationAdded(EZEventRelation Relation, FGameplayTag SelectedEvent)
{
	if (CurrentEvent == SelectedEvent)
	{
		UE_LOG(LogZEventEditor, Error, TEXT("Event can't be related to itself."));
		return;
	}
	FGameplayTagContainer& Container = CurrentDirectlyRelatedEvents[Relation];
	if (Container.HasTagExact(SelectedEvent))
	{
		UE_LOG(LogZEventEditor, Error, TEXT("%s is already related to %s (%s)"), *CurrentEvent.ToString(), *SelectedEvent.ToString(), *ENUM_TO_STRING(EZEventRelation, Relation));
		ensure(0);
		return;
	}
	Container.AddTag(SelectedEvent);
	switch (Relation)
	{
	case EZEventRelation::Required:
		OnEventRelationAdded(EZEventRelation::Required, CurrentEvent, SelectedEvent);
		break;
	case EZEventRelation::Blocked:
		OnEventRelationAdded(EZEventRelation::BlockedBy, SelectedEvent, CurrentEvent);
		break;
	case EZEventRelation::RequiredBy:
		OnEventRelationAdded(EZEventRelation::Required, SelectedEvent, CurrentEvent);
		break;
	case EZEventRelation::BlockedBy:
		OnEventRelationAdded(EZEventRelation::BlockedBy, CurrentEvent, SelectedEvent);
		break;
	}
	RebuildContentArea();
}

void SZEventEditor::OnEventRelationAdded(EZEventRelation Relation, FGameplayTag From, FGameplayTag To)
{
	switch (Relation)
	{
	case EZEventRelation::Required:
		{
			RelatedEvents[EZEventRelation::Required].FindOrAdd(From).AddTag(To);
			RelatedEvents[EZEventRelation::RequiredBy].FindOrAdd(To).AddTag(From);
			break;
		}
	case EZEventRelation::Blocked:
		{
			ensure(0);
			break;
		}
	case EZEventRelation::RequiredBy:
		{
			ensure(0);
			break;
		}
	case EZEventRelation::BlockedBy:
		{
			RelatedEvents[EZEventRelation::Blocked].FindOrAdd(To).AddTag(From);
			RelatedEvents[EZEventRelation::BlockedBy].FindOrAdd(From).AddTag(To);
			break;
		}
	}
	OnEventEdited(From);
}

void SZEventEditor::SetEditMode(bool bInEditMode)
{
	if (bEditMode == bInEditMode)
	{
		return;
	}
	bEditMode = bInEditMode;
	EditButton->SetVisibility(bEditMode ? EVisibility::Collapsed : EVisibility::Visible);
	RevertButton->SetVisibility(bEditMode ? EVisibility::Visible : EVisibility::Collapsed);
	RemoveButton->SetVisibility(bEditMode ? EVisibility::Visible : EVisibility::Collapsed);
	SaveButton->SetVisibility(bEditMode ? EVisibility::Visible : EVisibility::Collapsed);
}

FReply SZEventEditor::OnRefresh()
{
	LoadEvents();
	RefreshLists();
	NavigateTo(CurrentEvent);
	return FReply::Handled();
}

FReply SZEventEditor::OnInsertEventTag()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	MenuBuilder.AddWidget(BuildInsertEventTagWidget(), FText::GetEmpty());

	ShowPopupMenu(MenuBuilder);
	return FReply::Handled();
}

FReply SZEventEditor::OnInsert()
{
	if (EventTypes.Num() == 0)
	{
		ensure(0);
		UE_LOG(LogZEventEditor, Error, TEXT("No data types available."));
		return FReply::Handled();
	}
	NewEvent.Reset();
	FMenuBuilder MenuBuilder(true, nullptr);
	MenuBuilder.AddWidget(BuildInsertEventWidget(), FText::GetEmpty());

	ShowPopupMenu(MenuBuilder);
	return FReply::Handled();
}

FReply SZEventEditor::OnEdit()
{
	SetEditMode(true);
	RebuildContentArea();
	RebuildDetailsArea();
	return FReply::Handled();
}

FReply SZEventEditor::OnRevert()
{
	SetEditMode(false);
	
	TArray<UPackage*> DataTablePackages;
	for (const TPair<EZEventType, FZEventTypeData>& EventType : EventTypes)
	{
		for (const TPair<TObjectPtr<UDataTable>, FGameplayTagContainer>& Pair : EventType.Value.EventTables)
		{
			DataTablePackages.Add(Pair.Key.GetPackage());
		}
	}
	bool bOutAnyPackagesReloaded; 
	FText OutErrorMessage;
	UEditorLoadingAndSavingUtils::ReloadPackages(DataTablePackages, bOutAnyPackagesReloaded, OutErrorMessage, EReloadPackagesInteractionMode::AssumePositive);
	
	LoadEvents();
	NavigateTo(CurrentEvent);
	return FReply::Handled();
}

FReply SZEventEditor::Save()
{
	SetEditMode(false);
	RebuildContentArea();
	RebuildDetailsArea();
	
	for (const TPair<EZEventType, FZEventTypeData>& EventType : EventTypes)
	{
		for (const TPair<TObjectPtr<UDataTable>, FGameplayTagContainer>& Pair : EventType.Value.EventTables)
		{
			UPackage* Package = Pair.Key->GetPackage();
			const FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
			FSavePackageArgs SaveArgs;
			SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
			UPackage::SavePackage(Package, Pair.Key, *PackageFileName, SaveArgs);
		}
	}
	
	return FReply::Handled();
}

void SZEventEditor::ShowPopupMenu(FMenuBuilder MenuBuilder)
{
	HidePopupMenu();

	PopupMenu = FSlateApplication::Get().PushMenu(
		SharedThis(this),
		FWidgetPath(),
		MenuBuilder.MakeWidget(),
		FSlateApplication::Get().GetCursorPos(),
		FPopupTransitionEffect::ContextMenu
	);
}

void SZEventEditor::HidePopupMenu()
{
	if (PopupMenu.IsValid())
	{
		PopupMenu->Dismiss();
		PopupMenu.Reset();
	}
}

ECheckBoxState SZEventEditor::IsRepeatableFilterChecked() const
{
	return bShowRepeatable ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SZEventEditor::OnRepeatableFilterCheckStateChanged(ECheckBoxState NewState)
{
	bShowRepeatable = NewState == ECheckBoxState::Checked;
	RefreshLists();
}

ECheckBoxState SZEventEditor::IsIndirectFilterChecked() const
{
	return bShowIndirect ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SZEventEditor::OnIndirectFilterCheckStateChanged(ECheckBoxState NewState)
{
	bShowIndirect = NewState == ECheckBoxState::Checked;
	RebuildContentArea();
}

ECheckBoxState SZEventEditor::IsShowPreviewFullNamesChecked() const
{
	return bShowPreviewFullNames ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SZEventEditor::OnShowPreviewFullNamesCheckStateChanged(ECheckBoxState NewState)
{
	bShowPreviewFullNames = NewState == ECheckBoxState::Checked;
	OnEventPreview(CurrentTarget);
}

EZEventType SZEventEditor::GetEventType(FGameplayTag Event) const
{
	for (int32 Idx = 0; Idx < static_cast<uint8>(EZEventType::MAX); ++Idx)
	{
		const EZEventType Type = static_cast<EZEventType>(Idx);
		if (EventTypes[Type].Events.HasTagExact(Event))
		{
			return Type;
		}
	}
	ensure(0);
	return EZEventType::MAX;
}

void SZEventEditor::AddRelatedEventsList(TSharedRef<SVerticalBox> VBox, EZEventRelation Relation)
{
	VBox->AddSlot()
	    .AutoHeight()
	[
		SNew(SExpandableArea)
		.InitiallyCollapsed(false)
		.HeaderContent()
		[
			GetAreaHeaderContent(Relation)
		]
		.BodyContent()
		[
			GetEventList(Relation)
		]
	];
}

TSharedRef<SWidget> SZEventEditor::GetEventsAffecting()
{
	if (GetEventType(CurrentEvent) == EZEventType::Simple)
	{
		return SNullWidget::NullWidget; // Nothing affects simple events
	}
	TSharedRef<SVerticalBox> VBox = SNew(SVerticalBox);
	AddRelatedEventsList(VBox, EZEventRelation::Required);
	AddRelatedEventsList(VBox, EZEventRelation::BlockedBy);
	return VBox;
}

TSharedRef<SWidget> SZEventEditor::GetEventsAffectedBy()
{
	TSharedRef<SVerticalBox> VBox = SNew(SVerticalBox);
	AddRelatedEventsList(VBox, EZEventRelation::RequiredBy);
	AddRelatedEventsList(VBox, EZEventRelation::Blocked);
	return VBox;
}

TSharedRef<SWidget> SZEventEditor::GetEventList(EZEventRelation Relation)
{
	TSharedRef<SVerticalBox> Box = SNew(SVerticalBox);
	AddEventsToBox(Box, false, Relation, bEditMode);
	if (bShowIndirect)
	{
		AddEventsToBox(Box, true, Relation, false);
	}
	return Box;
}

void SZEventEditor::AddEventsToBox(TSharedRef<SVerticalBox> Box, bool bIndirect, EZEventRelation Relation, bool bEditable)
{
	const UZEventEditorSettings* Settings = GetDefault<UZEventEditorSettings>();

	TArray<FGameplayTag> SortedEvents;
	if (bIndirect)
	{
		CurrentIndirectlyRelatedEvents[Relation].GetGameplayTagArray(SortedEvents);
	}
	else
	{
		CurrentDirectlyRelatedEvents[Relation].GetGameplayTagArray(SortedEvents);
	}
	SortedEvents.Sort([](const FGameplayTag& A, const FGameplayTag& B)
	{
		return A.GetTagName().LexicalLess(B.GetTagName());
	});
	for (const FGameplayTag EventTag : SortedEvents)
	{
		Box->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				(bEditMode && bEditable)
					? ZEditorWidgetUtils::BuildButton(ZEditorText::Remove, [this, Relation, EventTag]()
					{
						OnCurrentEventRelationRemoved(Relation, EventTag);
					})
					: SNullWidget::NullWidget
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SSeparator)
				.Orientation(Orient_Vertical)
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(SZButton)
				.OnClicked(this, &SZEventEditor::OnEventPreview, EventTag)
				.OnDoubleClicked(this, &SZEventEditor::OnEventSelected, EventTag)
				.ForegroundColor(bIndirect ? Settings->IndirectEventColor : Settings->DirectEventColor)
				[
					SNew(STextBlock)
					.Text(FText::FromName(EventTag.GetTagName()))
				]
			]
		];
	}
}

TSharedRef<SWidget> SZEventEditor::GetAreaHeaderContent(EZEventRelation Relation)
{
	FText Title;
	FText Description;
	switch (Relation)
	{
	case EZEventRelation::Required:
		Title = LOCTEXT("RequiredEvents", "Required events");
		Description = FText::Format(LOCTEXT("RequiredEventsDetails", "Events that {0} requires"), FText::FromName(CurrentEvent.GetTagName()));
		break;
	case EZEventRelation::Blocked:
		Title = LOCTEXT("BlockedEvents", "Blocked events");
		Description = FText::Format(LOCTEXT("BlockedEventsDetails", "Events that {0} blocks"), FText::FromName(CurrentEvent.GetTagName()));
		break;
	case EZEventRelation::RequiredBy:
		Title = LOCTEXT("RequiredByEvents", "Required by events");
		Description = FText::Format(LOCTEXT("RequiredByEventsDetails", "Events that are required by {0}"), FText::FromName(CurrentEvent.GetTagName()));
		break;
	case EZEventRelation::BlockedBy:
		Title = LOCTEXT("BlockedByEvents", "Blocked by events");
		Description = FText::Format(LOCTEXT("BlockedByEventsDetails", "Events that are blocked by {0}"), FText::FromName(CurrentEvent.GetTagName()));
		break;
	}
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			bEditMode
				? ZEditorWidgetUtils::BuildButton(ZEditorText::Edit, [this, Relation]()
				{
					EditCurrentEventRelation(Relation);
				})
				: SNullWidget::NullWidget
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(Title)
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			.TransformPolicy(ETextTransformPolicy::ToUpper)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Bottom)
		.Padding(15.f, 0.f, 0.f, 0.f)
		[
			SNew(STextBlock)
			.Text(Description)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
		];
}

// Events that must be completed for SelectedEvent to be available
FGameplayTagContainer SZEventEditor::GetRequiredEvents(FGameplayTag SelectedEvent, bool bIndirect) const
{
	FGameplayTagContainer Visited;
	return GetRequiredEvents(SelectedEvent, Visited, bIndirect);
}

FGameplayTagContainer SZEventEditor::GetRequiredByEvents(FGameplayTag SelectedEvent, bool bIndirect) const
{
	FGameplayTagContainer Visited;
	return GetRequiredByEvents(SelectedEvent, Visited, bIndirect);
}

FGameplayTagContainer SZEventEditor::GetRequiredEvents(FGameplayTag SelectedEvent, FGameplayTagContainer& Visited, bool bIndirect) const
{
	if (Visited.HasTagExact(SelectedEvent))
	{
		return FGameplayTagContainer::EmptyContainer;
	}
	FGameplayTagContainer Result;
	if (const FGameplayTagContainer* RequiredBySelected = RelatedEvents[EZEventRelation::Required].Find(SelectedEvent))
	{
		Result.AppendTags(*RequiredBySelected);
		if (bIndirect)
		{
			for (const FGameplayTag EventRequiredBySelectedEvent : *RequiredBySelected)
			{
				// Get all events that are required by EventRequiredBySelectedEvent
				Result.AppendTags(GetRequiredEvents(EventRequiredBySelectedEvent, Visited, true));
			}
		}
	}
	return Result;
}

// Events that can only become available if SelectedEvent is completed
FGameplayTagContainer SZEventEditor::GetRequiredByEvents(FGameplayTag SelectedEvent, FGameplayTagContainer& Visited, bool bIndirect) const
{
	if (Visited.HasTagExact(SelectedEvent))
	{
		return FGameplayTagContainer::EmptyContainer;
	}
	FGameplayTagContainer Result;
	if (const FGameplayTagContainer* RequiresSelected = RelatedEvents[EZEventRelation::RequiredBy].Find(SelectedEvent))
	{
		Result.AppendTags(*RequiresSelected);
		if (bIndirect)
		{
			for (const FGameplayTag RequiresSelectedEvent : *RequiresSelected)
			{
				// Get events that can only become available if RequiresSelectedEvent is completed
				Result.AppendTags(GetRequiredByEvents(RequiresSelectedEvent, Visited, true));
			}
		}
	}
	return Result;
}

// Events that will become unavailable if SelectedEvent is completed
FGameplayTagContainer SZEventEditor::GetBlockedEvents(FGameplayTag SelectedEvent, bool bIndirect) const
{
	FGameplayTagContainer Result;
	if (const FGameplayTagContainer* BlockedBySelected = RelatedEvents[EZEventRelation::Blocked].Find(SelectedEvent))
	{
		Result.AppendTags(*BlockedBySelected);
		if (bIndirect)
		{
			// Get all events that require BlockedBySelectedEvent 
			for (const FGameplayTag BlockedBySelectedEvent : *BlockedBySelected)
			{
				Result.AppendTags(GetRequiredByEvents(BlockedBySelectedEvent, true));
			}
		}
	}
	return Result;
}

// Events that will make SelectedEvent unavailable if completed
FGameplayTagContainer SZEventEditor::GetBlockedByEvents(FGameplayTag SelectedEvent, bool bIndirect) const
{
	FGameplayTagContainer Result;
	if (const FGameplayTagContainer* BlocksSelected = RelatedEvents[EZEventRelation::BlockedBy].Find(SelectedEvent))
	{
		Result.AppendTags(*BlocksSelected);
	}
	if (bIndirect)
	{
		// Get all events that block any of SelectedEvents required events
		for (const FGameplayTag RequiredForSelectedEvent : GetRequiredEvents(SelectedEvent, true))
		{
			if (const FGameplayTagContainer* BlocksRequired = RelatedEvents[EZEventRelation::BlockedBy].Find(RequiredForSelectedEvent))
			{
				Result.AppendTags(*BlocksRequired);
			}
		}
	}
	return Result;
}

void SZEventEditor::ModifyDataTable(FGameplayTag EventID, TFunction<void()> EditFunction)
{
	if (UDataTable* DataTable = GetDataTable(EventID))
	{
		FDataTableEditorUtils::BroadcastPreChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowData);
		EditFunction();
		FDataTableEditorUtils::BroadcastPostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowData);
		const bool bSuccess = DataTable->MarkPackageDirty();
		if (!bSuccess)
		{
			UE_LOG(LogZEventEditor, Warning, TEXT("Failed to mark %s dirty"), *DataTable->GetName());
		}
	}
	else
	{
		ensure(0);
		UE_LOG(LogZEventEditor, Error, TEXT("Failed to find a data table for event %s"), *EventID.ToString());		
	}
}

void SZEventEditor::OnValueEdited(EZEditEventType Type)
{
	switch (Type) {
	case EZEditEventType::Regular:
		OnEventEdited(CurrentEvent);
		break;
	case EZEditEventType::Current:
		OnCurrentEventEdited();
		break;
	case EZEditEventType::Dialogue:
		OnDialogueEdited();
		break;
	case EZEditEventType::Simple:
		ensure(0);
		break;
	}
}

void SZEventEditor::OnEventEdited(FGameplayTag EditedRegularEvent)
{
	ensure(bEditMode);
	FZEventRow* EventRow = GetEventRow(EditedRegularEvent); // Need to retrieve the correct row here because we can edit other events than just the current one
	if (!EventRow)
	{
		UE_LOG(LogZEventEditor, Error, TEXT("Trying to edit an invalid event %s"), *EditedRegularEvent.ToString());
		return;
	}
	ModifyDataTable(EditedRegularEvent, [this, EditedRegularEvent, EventRow]()
	{
		EventRow->RequiredEvents = RelatedEvents[EZEventRelation::Required][EditedRegularEvent];
		EventRow->BlockedByEvents = RelatedEvents[EZEventRelation::BlockedBy][EditedRegularEvent];
	});
}

void SZEventEditor::OnCurrentEventEdited()
{
	ensure(bEditMode);
	FZEventRow* EventRow = GetEventRow(CurrentEvent);
	if (!EventRow)
	{
		UE_LOG(LogZEventEditor, Error, TEXT("Trying to edit an invalid event %s"), *CurrentEvent.ToString());
		return;
	}
	ModifyDataTable(CurrentEvent, [this, EventRow]()
	{
		EventRow->NPC = EditedEvent.NPC;
		EventRow->OnAvailable = EditedEvent.OnAvailable;
		EventRow->OnUnavailable = EditedEvent.OnUnavailable;
		EventRow->OnCompleted = EditedEvent.OnCompleted;
	});
}

void SZEventEditor::OnEventReactionEdited(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type)
{
	const int32 Index = GetEventReactionList(Type).IndexOfByKey(Reaction);
	if (ensure(Index != INDEX_NONE))
	{
		TInstancedStruct<FZEventReaction>& Target = GetEditedEventReactionList(Type)[Index];
		const UScriptStruct* ScriptStruct = Reaction->GetScriptStruct();
		Target.InitializeAsScriptStruct(ScriptStruct);
		ScriptStruct->CopyScriptStruct(Target.GetMutablePtr<FZEventReaction>(), Reaction->GetMemory());
		OnCurrentEventEdited();
	}
}

void SZEventEditor::OnDialogueEdited()
{
	ensure(bEditMode);
	if (!ensure(DialogueOptionsMap.Contains(CurrentEvent)))
	{
		return;
	}
	ModifyDataTable(CurrentEvent, [this]()
	{
		FZDialogueOptionRow* DialogueOptionRow = DialogueOptionsMap[CurrentEvent];
		DialogueOptionRow->Text = EditedDialogueOption.Text;
		DialogueOptionRow->Response = EditedDialogueOption.Response;
		DialogueOptionRow->bLocksDialogue = EditedDialogueOption.bLocksDialogue;
		DialogueOptionRow->ContinuationFrom = EditedDialogueOption.ContinuationFrom;
		DialogueOptionRow->bEndsDialogue = EditedDialogueOption.bEndsDialogue;
		DialogueOptionRow->DialogueContext = EditedDialogueOption.DialogueContext;
		DialogueOptionRow->bRepeatable = EditedDialogueOption.bRepeatable;
		DialogueOptionRow->Conditions = EditedDialogueOption.Conditions;
	});
}

void SZEventEditor::OnDialogueConditionEdited(TSharedPtr<FInstancedStruct> Condition)
{
	const int32 Index = EventConditionList.IndexOfByKey(Condition);
	if (ensure(Index != INDEX_NONE))
	{
		TInstancedStruct<FZEventCondition>& Target = EditedDialogueOption.Conditions[Index];
		const UScriptStruct* ScriptStruct = Condition->GetScriptStruct();
		Target.InitializeAsScriptStruct(ScriptStruct);
		ScriptStruct->CopyScriptStruct(Target.GetMutablePtr<FZEventCondition>(), Condition->GetMemory());
		OnDialogueEdited();
	}
}

FReply SZEventEditor::InsertEvent()
{
	const FString NewTagStr = FString::Printf(TEXT("%s.%s"), *NewEvent.RootTag.ToString(), *NewEvent.TagName.ToString());
	FGameplayTag NewTag = FGameplayTag::EmptyTag;
	if (IGameplayTagsEditorModule::Get().AddNewGameplayTagToINI(NewTagStr, NewEventComment.ToString()))
	{
		NewTag = UGameplayTagsManager::Get().RequestGameplayTag(FName(*NewTagStr));
		if (!NewTag.IsValid())
		{
			UE_LOG(LogZEventEditor, Error, TEXT("Error retrieving new tag: %s"), *NewTagStr);
			return FReply::Handled();
		}
	}
	else
	{
		UE_LOG(LogZEventEditor, Error, TEXT("Error retrieving new tag: %s"), *NewTagStr);
		return FReply::Handled();
	}
	if (!ensure(NewTag.IsValid()))
	{
		return FReply::Handled();
	}
	if (EventTypes[EZEventType::Regular].Events.HasTagExact(NewTag) 
		|| EventTypes[EZEventType::Dialogue].Events.HasTagExact(NewTag) 
		|| EventTypes[EZEventType::Timed].Events.HasTagExact(NewTag))
	{
		UE_LOG(LogZEventEditor, Error, TEXT("Trying to add %s but it already exists"), *NewTag.ToString());
		return FReply::Handled();
	}
	if (!NewTag.MatchesTag(ZGameplayTags::Event))
	{
		UE_LOG(LogZEventEditor, Error, TEXT("Invalid tag: %s"), *NewTag.ToString());
		return FReply::Handled();
	}
	HidePopupMenu();
	SetEditMode(true);
	
	if ((NewEvent.Type == EZEventType::Regular) || (NewEvent.Type == EZEventType::Dialogue))
	{
		if (!NewEvent.DataTable)
		{
			ensure(0);
			return FReply::Handled();
		}
		FName NewName = DataTableUtils::MakeValidName(TEXT("NewRow"));
		const TArray<FName> ExistingNames = NewEvent.DataTable->GetRowNames();
		while (ExistingNames.Contains(NewName))
		{
			NewName.SetNumber(NewName.GetNumber() + 1);
		}
		FZEventRow* NewRow = reinterpret_cast<FZEventRow*>(FDataTableEditorUtils::AddRow(NewEvent.DataTable, NewName));
		NewRow->ID = NewTag;
	
		// Try to automatically determine the NPC belonging to the event
		FGameplayTag NPC = FGameplayTag::EmptyTag;
		const UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
		TSharedPtr<FGameplayTagNode> NPCNode = GameplayTagsManager.FindTagNode(ZGameplayTags::NPC);
		if (ensure(NPCNode.IsValid()))
		{
			TArray<TSharedPtr<FGameplayTagNode>> NPCNodes = NPCNode->GetChildTagNodes();
			TSharedPtr<FGameplayTagNode> NewTagNode = GameplayTagsManager.FindTagNode(NewTag);
			while (NewTagNode.IsValid())
			{
				const int32 Idx = NPCNodes.IndexOfByPredicate([NewTagNode](const TSharedPtr<FGameplayTagNode>& NPCNode)
				{
					return NPCNode->GetSimpleTagName() == NewTagNode->GetSimpleTagName();
				});
				if (Idx != INDEX_NONE)
				{
					NPC = NPCNodes[Idx]->GetCompleteTag();
					// Don't break we want to match the node closes to the root
				}
				NewTagNode = NewTagNode->GetParentTagNode();
			}
		}
		NewRow->NPC = NPC;
	
		if (NewEvent.Type == EZEventType::Dialogue)
		{
			FZDialogueOptionRow* DialogueOptionRow = static_cast<FZDialogueOptionRow*>(NewRow);
			DialogueOptionRow->bRepeatable = DialogueOptionRow->ID.MatchesTag(ZGameplayTags::Event_Repeatable);
			
			FGameplayTag ParentTag = NewTag.RequestDirectParent();
			if (FZDialogueOptionRow** ParentDialogueOptionRow = DialogueOptionsMap.Find(ParentTag))
			{
				DialogueOptionRow->RequiredEvents.AddTag(ParentTag);
				if ((*ParentDialogueOptionRow)->bLocksDialogue)
				{
					DialogueOptionRow->ContinuationFrom = ParentTag;
				}
			}
		}
		LoadRegularEvent(NewRow, NewEvent.Type);
		EventTypes[NewEvent.Type].EventTables[NewEvent.DataTable].AddTag(NewTag);
	}
	else if (NewEvent.Type == EZEventType::Timed)
	{
		const FString TimedEventCompletedStr = FString::Printf(TEXT("%s.%s"), *NewTagStr, *ZGameplayTags::TimedEventCompletedLeafName.ToString());
		if (!IGameplayTagsEditorModule::Get().AddNewGameplayTagToINI(TimedEventCompletedStr, TEXT("")))
		{
			UE_LOG(LogZEventEditor, Error, TEXT("Error adding timed event completed tag for %s"), *NewTagStr);
		}
		const FString TimedEventExpiredStr = FString::Printf(TEXT("%s.%s"), *NewTagStr, *ZGameplayTags::TimedEventExpiredLeafName.ToString());
		if (!IGameplayTagsEditorModule::Get().AddNewGameplayTagToINI(TimedEventExpiredStr, TEXT("")))
		{
			UE_LOG(LogZEventEditor, Error, TEXT("Error adding timed event expired tag for %s"), *NewTagStr);
		}
		EventTypes[EZEventType::Timed].Events.AddTag(NewTag);
	}
	else if (NewEvent.Type == EZEventType::Simple)
	{
		EventTypes[EZEventType::Simple].Events.AddTag(NewTag);
	}
	
	RefreshLists();
	NavigateTo(NewTag);
	return FReply::Handled();
}

TArray<TSharedPtr<FGameplayTagNode>> SZEventEditor::GetChildNodes(TSharedPtr<FGameplayTagNode> Node) const
{
	TArray<TSharedPtr<FGameplayTagNode>> ChildNodes = { Node };
	for (TSharedPtr<FGameplayTagNode> ChildNode : Node->GetChildTagNodes())
	{
		ChildNodes.Append(GetChildNodes(ChildNode));
	}
	return ChildNodes;
}

void SZEventEditor::DeleteTags(const FGameplayTagContainer& TagsToDelete)
{
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	TSet<TSharedPtr<FGameplayTagNode>> NodesToDelete;
	FGameplayTagContainer AllDeletedTags;
	for (const FGameplayTag TagToDelete : TagsToDelete)
	{
		if (TSharedPtr<FGameplayTagNode> NodeToDelete = TagsManager.FindTagNode(TagToDelete))
		{
			AllDeletedTags.AddTag(TagToDelete);
			NodesToDelete.Add(NodeToDelete);
			TArray<TSharedPtr<FGameplayTagNode>> ChildNodes = GetChildNodes(NodeToDelete);
			for (TSharedPtr<FGameplayTagNode> ChildNode : ChildNodes)
			{
				AllDeletedTags.AddTag(ChildNode->GetCompleteTag());
			}
			NodesToDelete.Append(ChildNodes);
		}
		else
		{
			UE_LOG(LogZEventEditor, Error, TEXT("Couldn't find node for %s"), *TagToDelete.ToString());
		}
	}
	
	EventTypes[EZEventType::Simple].Events.RemoveTags(AllDeletedTags);
	EventTypes[EZEventType::Timed].Events.RemoveTags(AllDeletedTags);
	
	IGameplayTagsEditorModule::Get().DeleteTagsFromINI(NodesToDelete.Array());
	RefreshLists();
	if (AllDeletedTags.HasTagExact(CurrentEvent))
	{
		NavigateTo(FGameplayTag::EmptyTag);
	}
}

FReply SZEventEditor::RemoveEvent()
{
	EZEventType Type = GetEventType(CurrentEvent);
	if (UDataTable* Table = GetDataTable(CurrentEvent))
	{
		for (const TPair<FName, uint8*>& Pair : Table->GetRowMap())
		{
			const FZEventRow* Row = reinterpret_cast<FZEventRow*>(Pair.Value);
			if (Row->ID == CurrentEvent)
			{
				FDataTableEditorUtils::RemoveRow(Table, Pair.Key);
				break;
			}
		}
		EventTypes[Type].EventTables[Table].RemoveTag(CurrentEvent);
	}
	
	EventTypes[Type].Events.RemoveTag(CurrentEvent);
	
	if (Type == EZEventType::Regular)
	{
		EventsMap.Remove(CurrentEvent);
	}
	else if (Type == EZEventType::Dialogue)
	{
		DialogueOptionsMap.Remove(CurrentEvent);
	}
	
	RelatedEvents[EZEventRelation::Required].Remove(CurrentEvent);
	RelatedEvents[EZEventRelation::Blocked].Remove(CurrentEvent);
	RelatedEvents[EZEventRelation::RequiredBy].Remove(CurrentEvent);
	RelatedEvents[EZEventRelation::BlockedBy].Remove(CurrentEvent);
	
	// For now put it among the simple events, in the future we might just remove the tag entirely if no references are held
	EventTypes[EZEventType::Simple].Events.AddTag(CurrentEvent);
	
	RefreshLists();
	NavigateTo(FGameplayTag::EmptyTag);
	
	return FReply::Handled();
}

FZEventRow* SZEventEditor::GetEventRow(FGameplayTag EventID)
{
	if (FZEventRow** FoundRow = EventsMap.Find(EventID))
	{
		return *FoundRow;
	}
	else if (FZDialogueOptionRow** FoundDialogueRow = DialogueOptionsMap.Find(EventID))
	{
		return *FoundDialogueRow;
	}
	return nullptr;
}

UDataTable* SZEventEditor::GetDataTable(FGameplayTag EventID)
{
	EZEventType Type = GetEventType(EventID);
	for (const TPair<TObjectPtr<UDataTable>, FGameplayTagContainer>& Pair : EventTypes[Type].EventTables)
	{
		if (Pair.Value.HasTagExact(EventID))
		{
			return Pair.Key;
		}
	}
	return nullptr;
}

TSharedRef<SWidget> SZEventEditor::BuildRemoveTagsWidget()
{
	TagsToRemove.Reset();
	FGameplayTagContainer FilteredTags;
	FGameplayTagContainer RegularEvents;
	RegularEvents.AppendTags(EventTypes[EZEventType::Regular].Events);
	RegularEvents.AppendTags(EventTypes[EZEventType::Dialogue].Events);
	for (const FGameplayTag SimpleTag : EventTypes[EZEventType::Simple].Events)
	{
		if (!RegularEvents.HasTag(SimpleTag))
		{
			FilteredTags.AddTag(SimpleTag);
		}
	}
	for (const FGameplayTag TimedTag : EventTypes[EZEventType::Timed].Events)
	{
		if (!RegularEvents.HasTag(TimedTag))
		{
			FilteredTags.AddTag(TimedTag);
		}
	}
	return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SZGameplayTagPicker)
				.RootTag(ZGameplayTags::Event)
				.bMultiSelect(true)
				.FilteredTags(FilteredTags)
				.OnTagsChanged_Lambda([this](const FGameplayTagContainer& SelectedTags)
				{
					TagsToRemove = SelectedTags;
				})
				.OnClosed_Lambda([this]()
				{
					DeleteTags(TagsToRemove);
					TagsToRemove.Reset();
					HidePopupMenu();
				})
			];
}

void SZEventEditor::ValidateAll()
{
	for (int32 Idx = 0; Idx < static_cast<uint8>(EZEventType::MAX); ++Idx)
	{
		for (const FGameplayTag Event : EventTypes[static_cast<EZEventType>(Idx)].Events)
		{
			Validate(Event);
		}
	}
}

void SZEventEditor::Validate(FGameplayTag Event)
{
	if (FZEventRow* Row = GetEventRow(Event))
	{
		
	}
	if (FZDialogueOptionRow** FoundDialogueRow = DialogueOptionsMap.Find(Event))
	{
		FZDialogueOptionRow* DialogueRow = *FoundDialogueRow;
		if (DialogueRow->ContinuationFrom.IsValid() && DialogueRow->ContinuationFrom == DialogueRow->ID)
		{
			UE_LOG(LogZEventEditor, Error, TEXT("%s is continuing from itself"), *DialogueRow->ContinuationFrom.ToString());
		}
		if (DialogueRow->bLocksDialogue && DialogueRow->bEndsDialogue)
		{
			UE_LOG(LogZEventEditor, Error, TEXT("%s both locks and ends dialogue"), *DialogueRow->ID.ToString());
		}
	}
	UE_LOG(LogZEventEditor, Log, TEXT("Validating %s completed"), *Event.ToString());
}

#undef LOCTEXT_NAMESPACE

