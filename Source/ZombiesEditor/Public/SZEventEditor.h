// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "GameplayTagContainer.h"
#include "Editor/EditorWidgets/Public/SEnumCombo.h"
#include "Events/ZEvent.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Input/SComboBox.h"
#include "SZEventEditor.generated.h"

class IStructureDetailsView;
DECLARE_LOG_CATEGORY_EXTERN(LogZEventEditor, Log, All);

class SGameplayTagCombo;
class SZGameplayTagPickerButton;

UENUM()
enum class EZEventType : uint8
{
    Regular,
    Dialogue,
    Simple,
    Timed,
	MAX
};

enum class EZEditEventType : uint8
{
    Regular,
    Current,
    Dialogue,
    Simple
};

UENUM()
enum class EZEventRelation : uint8
{
    Required,
	Blocked,
	RequiredBy,
	BlockedBy,
};

UENUM()
enum class EZEventReactionType : uint8
{
    OnAvailable,
    OnUnavailable,
	OnCompleted
};

USTRUCT()
struct FZEventTypeData
{
	GENERATED_BODY()
	
	UPROPERTY()
	TMap<TObjectPtr<UDataTable>, FGameplayTagContainer> EventTables;
	FGameplayTagContainer Events;
};

USTRUCT()
struct FZNewEvent
{
	GENERATED_BODY()
	
	void Reset();
	FGameplayTag RootTag = FGameplayTag::EmptyTag;
	FText TagName = FText::GetEmpty();
	EZEventType Type = EZEventType::Simple;
	
	UPROPERTY()
	TObjectPtr<UDataTable> DataTable = nullptr;
};

struct FZEventConditionData
{
	UScriptStruct* Type;
	FText Text;
	TFunction<TSharedRef<SWidget>(TSharedPtr<FInstancedStruct>)> BuildEditableWidgetFunction;
};

struct FZEventReactionData
{
	UScriptStruct* Type;
	FText Text;
	TFunction<TSharedRef<SWidget>(TSharedPtr<FInstancedStruct>, EZEventReactionType)> BuildEditableWidgetFunction;
};

/**
 * 
 */
class ZOMBIESEDITOR_API SZEventEditor : public SCompoundWidget
{
public:
    SZEventEditor();

    SLATE_BEGIN_ARGS(SZEventEditor) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    void UpdateListSelection();
    void NavigateTo(FGameplayTag EventTag);
	void RebuildContentArea();
	void RebuildDetailsArea();
    TSharedRef<SWidget> BuildEventView();
    TSharedRef<SWidget> BuildDetailsPropertyNameWidget(const FText& PropertyName);
	TSharedRef<SWidget> BuildEditableTextWidget(FText& Text, EZEditEventType Type);
	TSharedRef<SWidget> BuildCheckboxWidget(bool bValue);
	TSharedRef<SWidget> BuildEditableCheckboxWidget(bool& bVariable, EZEditEventType Type);
	TSharedRef<SWidget> BuildEditableGameplayTagWidget(FGameplayTag& Event , FGameplayTag ParentTag, EZEditEventType Type, int32 FontSize = 12);
    template<typename TEnum>
	TSharedRef<SWidget> BuildEditableEnumWidget(TEnum& EnumValue, EZEditEventType Type);
	
	TSharedRef<SWidget> BuildRemoveReactionButton(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type);
    TSharedRef<SWidget> BuildEditableEventReactionWidget_ReceiveItem(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type);
    TSharedRef<SWidget> BuildEditableEventReactionWidget_GiveItem(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type);
    TSharedRef<SWidget> BuildEditableEventReactionWidget_GainXP(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type);
    TSharedRef<SWidget> BuildEditableEventReactionWidget_Reputation(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type);
    TSharedRef<SWidget> BuildEditableEventReactionWidget_HomeTransform(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type);
    TSharedRef<SWidget> BuildEditableEventReactionWidget_StartPath(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type);
    TSharedRef<SWidget> BuildEditableEventReactionWidget_CancelPath(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type);
    TSharedRef<SWidget> BuildEditableEventReactionWidget_StartTimedEvent(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type);
    TSharedRef<SWidget> BuildEditableEventReactionWidget_SetCanTrade(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type);
    TSharedRef<SWidget> CreateReactionTypeVBox(EZEventReactionType Type);
    TSharedRef<SWidget> BuildEditableEventReactionsWidget(EZEventReactionType Type);
	TArray<TSharedPtr<FInstancedStruct>>& GetEventReactionList(EZEventReactionType Type);
	TArray<TInstancedStruct<FZEventReaction>>& GetEditedEventReactionList(EZEventReactionType Type);
	TSharedPtr<SListView<TSharedPtr<FInstancedStruct>>>& GetEventReactionListView(EZEventReactionType Type);
	
	TSharedRef<SWidget> BuildRemoveConditionButton(TSharedPtr<FInstancedStruct> Condition);
	TSharedRef<SWidget> BuildEditableEventReputationConditionWidget(TSharedPtr<FInstancedStruct> Condition);
	TSharedRef<SWidget> BuildEditableEventInventoryConditionWidget(TSharedPtr<FInstancedStruct> Condition);
	TSharedRef<SWidget> BuildEditableEventSkillConditionWidget(TSharedPtr<FInstancedStruct> Condition);
    TSharedRef<SWidget> CreateConditionTypeVBox();
    TSharedRef<SWidget> BuildEditableEventConditionsWidget();
	
    TSharedRef<SWidget> BuildDialogueView();
    TSharedRef<SWidget> BuildEventChain(const TArray<FGameplayTag>& Tags, bool bBlocking) const;
    TArray<TArray<FGameplayTag>> CreateRequiredDependencyChains(FGameplayTag From, FGameplayTag To) const;
    TArray<TArray<FGameplayTag>> CreateRequiredDependencyChains(FGameplayTag From, FGameplayTag Current, FGameplayTag To) const;
    TArray<TArray<FGameplayTag>> CreateBlockedDependencyChains(FGameplayTag From, FGameplayTag To) const;
    TSharedRef<SWidget> BuildPreview(FGameplayTag Target);
	void UpdateNewEventWarning();
    TSharedRef<SWidget> BuildEventTypeComboBox();
	void RefreshFilteredEventTypes();
    TSharedRef<SWidget> BuildDataTableComboBox();
	void RefreshFilteredDataTables();
	TSharedRef<SWidget> BuildInsertEventTagWidget();
	TSharedRef<SWidget> BuildInsertEventWidget();
	void LoadRegularEvent(FZEventRow* EventRow, EZEventType Type);
    void LoadEvents();
    
	bool MatchesFilterWords(FGameplayTag EventTag, const TArray<FString> FilterWords) const;
	void RefreshLists();
	
    TSharedRef<ITableRow> OnGenerateEventRow(TSharedPtr<FGameplayTag> Item, const TSharedRef<STableViewBase>& OwnerTable);
    void OnEventSelectedFromList(TSharedPtr<FGameplayTag> Item, ESelectInfo::Type SelectInfo);
    FReply OnEventSelected(FGameplayTag Item);
    FReply OnEventPreview(FGameplayTag Item);
	
	FReply OnCurrentEventRelationRemoved(EZEventRelation Relation, FGameplayTag SelectedEvent);
	void OnEventRelationRemoved(EZEventRelation Relation, FGameplayTag From, FGameplayTag To);
	void EditCurrentEventRelation(EZEventRelation Relation);
	void OnCurrentEventRelationAdded(EZEventRelation Relation, FGameplayTag SelectedEvent);
	void OnEventRelationAdded(EZEventRelation Relation, FGameplayTag From, FGameplayTag To);

    void SetEditMode(bool bInEditMode);
	
    FReply OnRefresh();
	FReply OnInsertEventTag();
	FReply OnInsert();
	FReply OnEdit();
	FReply OnRevert();
	FReply Save();
	void ShowPopupMenu(FMenuBuilder MenuBuilder);
	void HidePopupMenu();
	
    ECheckBoxState IsRepeatableFilterChecked() const;
    void OnRepeatableFilterCheckStateChanged(ECheckBoxState NewState);
    ECheckBoxState IsIndirectFilterChecked() const;
    void OnIndirectFilterCheckStateChanged(ECheckBoxState NewState);
    ECheckBoxState IsShowPreviewFullNamesChecked() const;
    void OnShowPreviewFullNamesCheckStateChanged(ECheckBoxState NewState);

    EZEventType GetEventType(FGameplayTag Event) const;

	void AddRelatedEventsList(TSharedRef<SVerticalBox> VBox, EZEventRelation Relation);
    TSharedRef<SWidget> GetEventsAffecting();
    TSharedRef<SWidget> GetEventsAffectedBy();

	TSharedRef<SWidget> GetAreaHeaderContent(EZEventRelation Relation);
	
    TSharedRef<SWidget> GetEventList(EZEventRelation Relation);
    void AddEventsToBox(TSharedRef<SVerticalBox> Box, bool bIndirect, EZEventRelation Relation, bool bEditable);

    FGameplayTagContainer GetRequiredEvents(FGameplayTag SelectedEvent, bool bIndirect) const;
	FGameplayTagContainer GetRequiredByEvents(FGameplayTag SelectedEvent, bool bIndirect) const;

    FGameplayTagContainer GetRequiredEvents(FGameplayTag SelectedEvent, FGameplayTagContainer& Visited, bool bIndirect) const;
    FGameplayTagContainer GetRequiredByEvents(FGameplayTag SelectedEvent, FGameplayTagContainer& Visited, bool bIndirect) const;
    FGameplayTagContainer GetBlockedEvents(FGameplayTag SelectedEvent, bool bIndirect) const;
    FGameplayTagContainer GetBlockedByEvents(FGameplayTag SelectedEvent, bool bIndirect) const;
	
	void ModifyDataTable(FGameplayTag EventID, TFunction<void()> EditFunction);
	void OnValueEdited(EZEditEventType Type);
	void OnEventEdited(FGameplayTag EditedEvent);
	void OnCurrentEventEdited();
	void OnEventReactionEdited(TSharedPtr<FInstancedStruct> Reaction, EZEventReactionType Type);
	void OnDialogueEdited();
	void OnDialogueConditionEdited(TSharedPtr<FInstancedStruct> Condition);
	FReply InsertEvent();
	TArray<TSharedPtr<FGameplayTagNode>> GetChildNodes(TSharedPtr<FGameplayTagNode> Node) const;
	void DeleteTags(const FGameplayTagContainer& TagsToDelete);
	FReply RemoveEvent();
	
	FZEventRow* GetEventRow(FGameplayTag EventID);
	UDataTable* GetDataTable(FGameplayTag EventID);
	
	TSharedRef<SWidget> BuildRemoveTagsWidget();
	void ValidateAll();
	void Validate(FGameplayTag Event);
	
    // Filtered subset shown in the list
    TArray<TSharedPtr<FGameplayTag>> FilteredEvents;
    // Filtered subset shown in the list
    TArray<TSharedPtr<FGameplayTag>> FilteredDialogueOptions;
    // Filtered subset shown in the list
    TArray<TSharedPtr<FGameplayTag>> FilteredSimpleEvents;
    // Filtered subset shown in the list
    TArray<TSharedPtr<FGameplayTag>> FilteredTimedEvents;

	// All Events and their related events
	TMap<EZEventRelation, TMap<FGameplayTag, FGameplayTagContainer>> RelatedEvents;
	
	// Current event related events
	TMap<EZEventRelation, FGameplayTagContainer> CurrentDirectlyRelatedEvents;
	TMap<EZEventRelation, FGameplayTagContainer> CurrentIndirectlyRelatedEvents;
	
	// Assigned widgets
    TSharedPtr<SScrollBox> ListScrollBox;
    TSharedPtr<SListView<TSharedPtr<FGameplayTag>>> EventListView;
    TSharedPtr<SListView<TSharedPtr<FGameplayTag>>> DialogueOptionListView;
    TSharedPtr<SListView<TSharedPtr<FGameplayTag>>> SimpleEventListView;
    TSharedPtr<SListView<TSharedPtr<FGameplayTag>>> TimedEventListView;
	TSharedPtr<SSearchBox> SearchBox;
	TSharedPtr<SButton> InsertButton;
	TSharedPtr<SButton> EditButton;
	TSharedPtr<SButton> RemoveButton;
	TSharedPtr<SButton> RevertButton;
	TSharedPtr<SButton> SaveButton;
	TSharedPtr<STextBlock> InsertWidgetWarningText;
	TSharedPtr<SComboBox<TSharedPtr<EZEventType>>> EventTypeComboBox;
	TSharedPtr<SComboBox<TWeakObjectPtr<UDataTable>>> DataTableComboBox;
	TSharedPtr<SListView<TSharedPtr<FInstancedStruct>>> EventConditionListView;
	TSharedPtr<SListView<TSharedPtr<FInstancedStruct>>> EventOnAvailableReactionListView;
	TSharedPtr<SListView<TSharedPtr<FInstancedStruct>>> EventOnUnavailableReactionListView;
	TSharedPtr<SListView<TSharedPtr<FInstancedStruct>>> EventOnCompletedReactionListView;

	TSharedPtr<SBox> ContentArea;
	TSharedPtr<SBox> DetailsArea;
	TSharedPtr<SBox> PreviewArea;
	
	TSharedPtr<IMenu> PopupMenu;
	
	TSharedPtr<IStructureDetailsView> StructureDetailsView;
	TSharedPtr<SEditableTextBox> NewEventNameBox;
	
	// Assigned widget data
    FGameplayTag CurrentEvent;
    FGameplayTag CurrentTarget;
    bool bShowRepeatable;
    bool bShowIndirect;
	bool bShowPreviewFullNames;
	bool bEditMode;
	FGameplayTag AddedRelationTag;
	FGameplayTag NewEventRootTag;
	FText NewEventText;
	FText NewEventComment;
	FGameplayTagContainer TagsToRemove;
	
	TMap<EZEventType, FZEventTypeData> EventTypes;
	TArray<TSharedPtr<EZEventType>> FilteredEventTypes;
	TArray<TWeakObjectPtr<UDataTable>> FilteredDataTables;
    TMap<FGameplayTag, FZEventRow*> EventsMap;
    TMap<FGameplayTag, FZDialogueOptionRow*> DialogueOptionsMap;
	
	FZEventRow EditedEvent;
	FZDialogueOptionRow EditedDialogueOption;
	FZNewEvent NewEvent;
	
	TArray<TSharedPtr<FInstancedStruct>> EventConditionList;
	TArray<TSharedPtr<FInstancedStruct>> EventOnAvailableReactionList;
	TArray<TSharedPtr<FInstancedStruct>> EventOnUnavailableReactionList;
	TArray<TSharedPtr<FInstancedStruct>> EventOnCompletedReactionList;
	
	TArray<FZEventConditionData> ConditionData;
	TArray<FZEventReactionData> ReactionData;
};

template <typename TEnum>
TSharedRef<SWidget> SZEventEditor::BuildEditableEnumWidget(TEnum& EnumValue, EZEditEventType Type)
{
	static_assert(TIsUEnumClass<TEnum>::Value, "TEnum must be a UENUM");
	if (bEditMode)
	{
		return SNew(SEnumComboBox, StaticEnum<TEnum>())
				.CurrentValue_Lambda([&EnumValue]()
				{
					return static_cast<int32>(EnumValue);
				})
				.OnEnumSelectionChanged_Lambda([this, &EnumValue, Type](int32 NewValue, ESelectInfo::Type)
				{
					EnumValue = static_cast<TEnum>(NewValue);
					OnValueEdited(Type);
				});
	}
	else
	{
		return SNew(STextBlock)
				.Text(StaticEnum<TEnum>()->GetDisplayValueAsText(EnumValue));
	}
}


