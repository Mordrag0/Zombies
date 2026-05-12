// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZTradeWidget.h"
#include "Characters/ZNPCharacter.h"
#include "UI/ZTradeInventoryWidget.h"
#include "UI/ZButton.h"
#include "ZPlayerController.h"
#include "CommonTextBlock.h"
#include "UI/ZInventoryWidget.h"
#include "Inventory/ZInventoryComponent.h"
#include "UI/ZTradeItemWidget.h"
#include "UI/ZHUDWidget.h"

void UZTradeWidget::StartTrade(AZNPCharacter* InNPC)
{
	if (!InNPC || !GetOwningPlayerPawn<AZCharacter>())
	{
		return;
	}
	NPC = InNPC;
	UZInventoryComponent* PlayerInventoryComponent = GetOwningPlayerPawn<AZCharacter>()->GetInventoryComponent();
	UZInventoryComponent* NPCInventoryComponent = NPC->GetInventoryComponent();
	PlayerInventory->Refresh(PlayerInventoryComponent);
	NPCInventory->Refresh(NPCInventoryComponent);

	BIND_MULTICAST_UOBJECT(OnPlayerInventoryChangedConnection, PlayerInventoryComponent, PlayerInventoryComponent->OnInventoryChanged, this, &ThisClass::Reset);
	BIND_MULTICAST_UOBJECT(OnNPCInventoryChangedConnection, NPCInventoryComponent, NPCInventoryComponent->OnInventoryChanged, this, &ThisClass::Reset);
}

void UZTradeWidget::UpdateCost()
{
	const int32 Cost = GetCurrentCost();
	TradeCost->SetText(FText::AsNumber(Cost));

	if (Cost < 0)
	{
		const int32 Coins = PlayerInventory->GetInventoryWidget()->GetInventoryComponent()->GetCoins();
		AcceptButton->SetIsEnabled(FMath::Abs(Cost) <= Coins);
	}
	else
	{
		const int32 Coins = NPCInventory->GetInventoryWidget()->GetInventoryComponent()->GetCoins();
		AcceptButton->SetIsEnabled(Cost <= Coins);
	}
}

int32 UZTradeWidget::GetCurrentCost() const
{
	return PlayerInventory->GetTradeCost() - NPCInventory->GetTradeCost();
}

void UZTradeWidget::NativeOnDeactivated()
{
	OnPlayerInventoryChangedConnection.Disconnect();
	OnNPCInventoryChangedConnection.Disconnect();

	Super::NativeOnDeactivated();
}

void UZTradeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	AcceptButton->OnClicked().AddUObject(this, &ThisClass::Accept);
	ResetButton->OnClicked().AddUObject(this, &ThisClass::Reset);
	DeclineButton->OnClicked().AddUObject(this, &ThisClass::RemoveFromStack);
}

void UZTradeWidget::Accept()
{
	FZTradeRequest TradeRequest;
	TradeRequest.Cost = GetCurrentCost();
	TradeRequest.NPCInventory = NPCInventory->GetInventoryWidget()->GetInventoryComponent();

	const TArray<UZTradeItemWidget*> PlayerTradedItems = PlayerInventory->GetInventoryWidget()->GetAllItemWidgets<UZTradeItemWidget>();
	for (const UZTradeItemWidget* ItemWidget : PlayerTradedItems)
	{
		if (ItemWidget->GetSelectedCount() > 0)
		{
			TradeRequest.PlayerItems.Add(FZItemStack(ItemWidget->GetItemId(), ItemWidget->GetSelectedCount()));
		}
	}
	const TArray<UZTradeItemWidget*> NPCTradedItems = NPCInventory->GetInventoryWidget()->GetAllItemWidgets<UZTradeItemWidget>();
	for (const UZTradeItemWidget* ItemWidget : NPCTradedItems)
	{
		if (ItemWidget->GetSelectedCount() > 0)
		{
			TradeRequest.NPCItems.Add(FZItemStack(ItemWidget->GetItemId(), ItemWidget->GetSelectedCount()));
		}
	}

	PlayerInventory->GetInventoryWidget()->GetInventoryComponent()->AcceptTrade(TradeRequest, false);
	Reset();
}

void UZTradeWidget::Reset()
{
	StartTrade(NPC);
	UpdateCost();
}


