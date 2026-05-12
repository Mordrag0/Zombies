// Copyright 2026 Luka Markuš. All rights reserved.


#include "SZCharacterStateWidget.h"
#include "ZEditorWidgetUtils.h"
#include "ZTypes.h"
#include "Characters/ZCharacter.h"
#include "Characters/ZNPCharacter.h"


void SZCharacterStateWidget::Construct(const FArguments& InArgs)
{
	Character = InArgs._Character;
	
	static FSlateRoundedBoxBrush BorderBrush(
		FLinearColor::Black,  // background
		3.f,                  // corner radius
		FLinearColor::White,  // border color
		1.f                   // border width
	);
	
	ChildSlot
	.Padding(2.f)
	[
		SNew(SBorder)
		.BorderImage(&BorderBrush)
		.Padding(8.f)
		[
			SAssignNew(ContentBox, SVerticalBox)
		]
	];

	const EZCharacterActivity Activities = Character->GetCurrentActivities();
	for (uint32 Bit = 1; Bit < (uint32)EZCharacterActivity::MAX; Bit <<= 1)
	{
		if (EnumHasAnyFlags(Activities, (EZCharacterActivity)Bit))
		{
			CurrentActivities.Add(MakeShared<EZCharacterActivity>((EZCharacterActivity)Bit));
		}
	}
	
	RebuildContent();
	
	Character->OnInteractionActivityStarted.AddSP(this, &SZCharacterStateWidget::OnActivityStarted);	
	Character->OnActivityEnding.AddSP(this, &SZCharacterStateWidget::OnActivityEnding);
	Character->OnActivityStopped.AddSP(this, &SZCharacterStateWidget::OnActivityStopped);
}

SZCharacterStateWidget::~SZCharacterStateWidget()
{
	if (Character.IsValid())
	{
		Character->OnInteractionActivityStarted.RemoveAll(this);	
		Character->OnActivityEnding.RemoveAll(this);
		Character->OnActivityStopped.RemoveAll(this);
	}
}

void SZCharacterStateWidget::RebuildContent()
{
	ContentBox->ClearChildren();

	if (!Character.IsValid())
	{
		ContentBox->AddSlot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Invalid character"))
					];
		return;
	}

	FText NameText = FText::FromString(Character->GetName());
	if (AZNPCharacter* NPCharacter = Cast<AZNPCharacter>(Character))
	{
		NameText = FText::FromName(NPCharacter->GetNPCName());
	}
	ContentBox->AddSlot()
				.AutoHeight()
				[
					SNew(SButton)
					.Text(NameText)
					.OnClicked_Lambda([this]()
					{
						ZEditorWidgetUtils::EditorSelectActor(Character.Get());
						return FReply::Handled();
					})
				];
	ContentBox->AddSlot()
				.AutoHeight()
				[
					SAssignNew(ActivityList, SListView<TSharedPtr<EZCharacterActivity>>)
					.ListItemsSource(&CurrentActivities)
					.SelectionMode(ESelectionMode::None)
					.OnGenerateRow_Lambda([this](TSharedPtr<EZCharacterActivity> Item, const TSharedRef<STableViewBase>& OwnerTable)
					{
						if (!Character.IsValid())
						{
							return SNew(STableRow<TSharedPtr<EZCharacterActivity>>, OwnerTable);
						}
						const FText Text = FText::FromString(ENUM_TO_STRING(EZCharacterActivity, *Item));
						const bool bEnding = Character->IsEndingActivity(*Item);
						AActor* Interactable = Cast<AActor>(Character->GetInteractionTarget(*Item).GetObject());
						FText InteractableName = Interactable ? FText::FromString(Interactable->GetName()) : FText::GetEmpty();
						return SNew(STableRow<TSharedPtr<EZCharacterActivity>>, OwnerTable)
							[
								SNew(SBorder)
								.BorderBackgroundColor(bEnding ? FLinearColor(0.7f, 0.2f, 0.2f, 1.f) : FLinearColor(0.2f, 0.2f, 0.2f, 1.f))
								[
									SNew(SHorizontalBox)
									+SHorizontalBox::Slot()
									.FillWidth(1.f)
									[
										SNew(STextBlock)
										.Text(Text)
										.Margin(FMargin(4.0f, 2.0f))
									]
									+SHorizontalBox::Slot()
									.FillWidth(1.f)
									[
										SNew(SButton)
										.Text(InteractableName)
										.ContentPadding(FMargin(4.0f, 2.0f))
										.OnClicked_Lambda([WeakInteractable = MakeWeakObjectPtr(Interactable)]()
										{
											ZEditorWidgetUtils::EditorSelectActor(WeakInteractable.Get());
											return FReply::Handled();
										})
									]
								]
							];
					})
				];
	
}

void SZCharacterStateWidget::OnActivityStarted(const FZInteractionState& InteractionState)
{
	CurrentActivities.Add(MakeShared<EZCharacterActivity>(InteractionState.Activity));
	ActivityList->RequestListRefresh();
}

void SZCharacterStateWidget::OnActivityEnding(EZCharacterActivity Activity)
{
	ActivityList->RequestListRefresh();
}

void SZCharacterStateWidget::OnActivityStopped(EZCharacterActivity Activity)
{
	CurrentActivities.RemoveAll([Activity](TSharedPtr<EZCharacterActivity> Item)
	{
		return Item.IsValid() && (*Item == Activity);
	});
	ActivityList->RequestListRefresh();
}

