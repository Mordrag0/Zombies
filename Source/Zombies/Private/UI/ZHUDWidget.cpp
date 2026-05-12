// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZHUDWidget.h"
#include "UI/ZDialogueWidget.h"
#include "UI/ZTradeWidget.h"
#include "Characters/ZCharacter.h"
#include "Characters/ZFPCharacter.h"
#include "ZPlayerController.h"
#include "UI/ZLootWidget.h"
#include "UI/ZAmmoWidget.h"
#include "UI/ZHealthWidget.h"
#include "UI/ZStaminaWidget.h"
#include "UI/ZQuestWidget.h"
#include "UI/ZSkillWidget.h"
#include "UI/ZMapWidget.h"
#include "UI/ZPauseMenuWidget.h"
#include "UI/ZHUD.h"
#include "UI/ZPlayerInventoryWidget.h"

void UZHUDWidget::Init(AZFPCharacter* InCharacter)
{
	if (AmmoWidget)
	{
		AmmoWidget->Init(InCharacter);
	}
	if (HealthBar)
	{
		HealthBar->Init(InCharacter);
	}
	if (StaminaBar)
	{
		StaminaBar->Init(InCharacter);
	}
}

void UZHUDWidget::ShowInventory(UZInventoryComponent* Inventory)
{
	ShowActivatableWidget<UZPlayerInventoryWidget>(InventoryWidget, InventoryWidgetClass, EZActivatableWidgetType::Game);
	if (InventoryWidget)
	{
		InventoryWidget->Open(Inventory);
	}
}

void UZHUDWidget::ShowDialogueWidget()
{
	ShowActivatableWidget<UZDialogueWidget>(DialogueWidget, DialogueWidgetClass, EZActivatableWidgetType::Game);
}

AZHUD* UZHUDWidget::GetHUD() const
{
	return GetOwningPlayer<AZPlayerController>()->GetHUD<AZHUD>();
}

void UZHUDWidget::StartDialogue(AZNPCharacter* NPC, const FZDialogueParams& DialogueParams)
{
	ShowDialogueWidget();
	PushDialogueOptions(NPC, DialogueParams);
}

void UZHUDWidget::StartDialogue(const FZDialogueOptionRow* NPCInitiatedRow)
{
	ShowDialogueWidget();
	ShowResponse(NPCInitiatedRow);
}

void UZHUDWidget::PushDialogueOptions(AZNPCharacter* NPC, const FZDialogueParams& DialogueParams)
{
	if (DialogueWidget)
	{
		DialogueWidget->PushDialogueOptions(NPC, DialogueParams);
	}
}

void UZHUDWidget::RefreshDialogueOptions()
{
	if (DialogueWidget)
	{
		DialogueWidget->RefreshDialogueOptions();
	}
}

void UZHUDWidget::ShowResponse(const FZDialogueOptionRow* Row)
{
	if (DialogueWidget)
	{
		DialogueWidget->ShowDialogueResponse(Row);
	}
}

void UZHUDWidget::EndDialogue()
{
	if (DialogueWidget)
	{
		DialogueWidget->RemoveFromStack();
	}
}

void UZHUDWidget::StartTrade(AZNPCharacter* NPC)
{
	ShowActivatableWidget<UZTradeWidget>(TradeWidget, TradeWidgetClass, EZActivatableWidgetType::Game);
	if (TradeWidget)
	{
		TradeWidget->StartTrade(NPC);
	}
}

void UZHUDWidget::UpdateTradeCost()
{
	if (TradeWidget)
	{
		TradeWidget->UpdateCost();
	}
}

void UZHUDWidget::StartLooting(UZInventoryComponent* Inventory)
{
	ShowActivatableWidget<UZLootWidget>(LootWidget, LootWidgetClass, EZActivatableWidgetType::Game);
	if (LootWidget)
	{
		LootWidget->StartLooting(Inventory);
	}
}

void UZHUDWidget::StopLooting()
{
	// #ZTODO do we need this? what happens if interrupted?
	if (LootWidget)
	{
		LootWidget->RemoveFromStack();
	}
}

void UZHUDWidget::ShowQuests()
{
	ShowActivatableWidget<UZQuestWidget>(QuestWidget, QuestWidgetClass, EZActivatableWidgetType::Game);
}

void UZHUDWidget::ShowSkills()
{
	ShowActivatableWidget<UZSkillWidget>(SkillsWidget, SkillsWidgetClass, EZActivatableWidgetType::Game);
}

void UZHUDWidget::ShowMap()
{
	ShowActivatableWidget<UZMapWidget>(MapWidget, MapWidgetClass, EZActivatableWidgetType::Game);
}

void UZHUDWidget::StartLockpicking(AZLock* Lock)
{
	//ShowActivatableWidget<UZLockpickWidget>(LockpickWidget, LockpickWidgetClass, EZWidgetType::Game);
}

void UZHUDWidget::StopLockpicking()
{
}

void UZHUDWidget::ShowPauseWidget()
{
	ShowActivatableWidget<UZPauseMenuWidget>(PauseMenuWidget, PauseMenuWidgetClass, EZActivatableWidgetType::Menu);
	if (PauseMenuWidget)
	{
		AZPlayerController* PC = GetOwningPlayer<AZPlayerController>();
		BIND_MULTICAST(UnpauseConnection, PauseMenuWidget, PauseMenuWidget->OnRemoved, 
			[WeakPC = MakeWeakObjectPtr(PC), WeakThis = MakeWeakObjectPtr(this)]()
		{
			if (WeakPC.IsValid())
			{
				WeakPC->Unpause();
			}
			if (WeakThis.IsValid())
			{
				WeakThis->UnpauseConnection.Disconnect();
			}
		});
	}
}

void UZHUDWidget::NativeDestruct()
{
	WidgetDeactivatedConnections.Empty();
	UnpauseConnection.Disconnect();

	Super::NativeDestruct();
}

TOptional<FUIInputConfig> UZHUDWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::CapturePermanently);
}



