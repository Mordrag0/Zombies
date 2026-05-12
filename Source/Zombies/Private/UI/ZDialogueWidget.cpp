// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZDialogueWidget.h"
#include "Blueprint/UserWidget.h"
#include "UI/ZDialogueRowWidget.h"
#include "Components/VerticalBox.h"
#include "ZGameMode.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/ZDialogueResponseRowWidget.h"
#include "Characters/ZNPCharacter.h"
#include "UI/ZTradeRowWidget.h"
#include "ZGameState.h"
#include "ZPlayerController.h"
#include "UI/ZEndDialogueRowWidget.h"
#include "Player/ZPlayerState.h"
#include "Components/ZSkillsComponent.h"
#include "UI/ZRowWidget.h"
#include "UI/ZTeachRowWidget.h"
#include "AI/ZNPCAIController.h"

void UZDialogueWidget::PushDialogueOptions(AZNPCharacter* NPC, const FZDialogueParams& DialogueParams)
{
	if (DialogueWidgetState != EZDialogueWidgetState::WaitingForOptions)
	{
		if (ensure(DialogueWidgetState != EZDialogueWidgetState::PendingOptions)) // Make sure we're not pending two sets of options
		{
			PendingNPC = NPC;
			PendingDialogueParams = DialogueParams;
			DialogueWidgetState = EZDialogueWidgetState::PendingOptions;
		}
		return;
	}
	DialogueWidgetState = EZDialogueWidgetState::None;

	auto AddRowToBox = [this](UZRowWidget* Widget)
	{
		UVerticalBoxSlot* Slot = VerticalBox->AddChildToVerticalBox(Widget);
		Slot->SetHorizontalAlignment(HAlign_Fill);
		Slot->SetVerticalAlignment(VAlign_Fill);
	};
	VerticalBox->ClearChildren();

	AZGameState* GS = GetWorld()->GetGameState<AZGameState>();
	if (!ensure(GS))
	{
		return;
	}

	const TArray<const FZDialogueOptionRow*> DialogueOptions = GS->GetDialogueOptions(DialogueParams);
	for (const FZDialogueOptionRow* DialogueOption : DialogueOptions)
	{
		UZDialogueRowWidget* RowWidget = CreateWidget<UZDialogueRowWidget>(this, DialogueRowClass);
		RowWidget->Init(DialogueOption);
		AddRowToBox(RowWidget);
	}

	if ((DialogueParams.DialogueContext == EZDialogueContext::None) && !DialogueParams.bDialogueLocked)
	{
		const TSet<EZSkill>& TeachableSkills = NPC->GetTeachableSkills();
		const AZPlayerState* PS = GetOwningPlayer()->GetPlayerState<AZPlayerState>();
		const UZSkillsComponent* SkillsComp = PS->GetSkillsComponent();
		for (const EZSkill Skill : TeachableSkills)
		{
			if (!SkillsComp->HasSkill(Skill))
			{
				UZTeachRowWidget* TeachRowWidget = CreateWidget<UZTeachRowWidget>(this, TeachRowClass);
				TeachRowWidget->Init(Skill);
				AddRowToBox(TeachRowWidget);
			}
		}

		if (NPC->CanTrade())
		{
			UZTradeRowWidget* TradeRowWidget = CreateWidget<UZTradeRowWidget>(this, TradeRowClass);
			TradeRowWidget->Init(NPC);
			AddRowToBox(TradeRowWidget);
		}

		UZEndDialogueRowWidget* EndRowWidget = CreateWidget<UZEndDialogueRowWidget>(this, EndRowClass);
		AddRowToBox(EndRowWidget);
	}
}

void UZDialogueWidget::ShowDialogueResponse(const FZDialogueOptionRow* Row)
{
	VerticalBox->ClearChildren();

	UZDialogueResponseRowWidget* ResponseRow = CreateWidget<UZDialogueResponseRowWidget>(this, ResponseRowClass);
	ResponseRow->Init(Row);
	UVerticalBoxSlot* VBSlot = VerticalBox->AddChildToVerticalBox(ResponseRow);
	VBSlot->SetHorizontalAlignment(HAlign_Fill);
	VBSlot->SetVerticalAlignment(VAlign_Fill);
}

void UZDialogueWidget::RefreshDialogueOptions()
{
	if (DialogueWidgetState == EZDialogueWidgetState::PendingOptions)
	{
		DialogueWidgetState = EZDialogueWidgetState::WaitingForOptions;
		PushDialogueOptions(PendingNPC, PendingDialogueParams);
		PendingNPC = nullptr;
		PendingDialogueParams = FZDialogueParams();
	}
	else
	{
		VerticalBox->ClearChildren();
		ensure(DialogueWidgetState == EZDialogueWidgetState::None);
		DialogueWidgetState = EZDialogueWidgetState::WaitingForOptions;
	}
}

void UZDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(DialogueRowClass);
	ensure(TradeRowClass);
	ensure(TeachRowClass);
	ensure(EndRowClass);
	ensure(ResponseRowClass);

	DialogueWidgetState = EZDialogueWidgetState::WaitingForOptions;
}

void UZDialogueWidget::NativeDestruct()
{
	DialogueWidgetState = EZDialogueWidgetState::None;

	Super::NativeDestruct();
}


