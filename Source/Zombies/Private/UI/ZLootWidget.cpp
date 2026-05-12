// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZLootWidget.h"
#include "UI/ZButton.h"
#include "Characters/ZCharacter.h"
#include "UI/ZLootInventoryWidget.h"
#include "ZPlayerController.h"
#include "Inventory/ZInventoryComponent.h"
#include "UI/ZInventoryWidget.h"

void UZLootWidget::StartLooting(UZInventoryComponent* InInventory)
{
	if (!InInventory || !GetOwningPlayerPawn<AZCharacter>())
	{
		return;
	}
	ContainerInventoryComponent = InInventory;
	UZInventoryComponent* PlayerInventoryComponent = GetOwningPlayerPawn<AZCharacter>()->GetInventoryComponent();
	PlayerInventory->Refresh(PlayerInventoryComponent);
	ContainerInventory->Refresh(InInventory);
	
	BIND_MULTICAST_UOBJECT(OnPlayerInventoryChangedConnection, PlayerInventoryComponent, PlayerInventoryComponent->OnInventoryChanged, this, &ThisClass::Refresh);
	BIND_MULTICAST_UOBJECT(OnContainerInventoryChangedConnection, PlayerInventoryComponent, PlayerInventoryComponent->OnInventoryChanged, this, &ThisClass::Refresh);
}

void UZLootWidget::Move(FPrimaryAssetId ItemId, int32 Count, EZLootWidgetType Type)
{
	if (Type == EZLootWidgetType::Player)
	{
		return;
	}
	FZLootRequest LootRequest;
	LootRequest.NPCItems.Add(ItemId);
	LootRequest.ContainerInventory = ContainerInventory->GetInventoryWidget()->GetInventoryComponent();
	PlayerInventory->GetInventoryWidget()->GetInventoryComponent()->Loot(LootRequest);
}

void UZLootWidget::NativeOnDeactivated()
{
	OnPlayerInventoryChangedConnection.Disconnect();
	OnContainerInventoryChangedConnection.Disconnect();
	 
	Super::NativeOnDeactivated();
}

void UZLootWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ExitButton->OnClicked().AddUObject(this, &ThisClass::RemoveFromStack);
	TakeAllButton->OnClicked().AddUObject(this, &ThisClass::TakeAll);
}

void UZLootWidget::RemoveFromStack()
{
	Super::RemoveFromStack();

	AZPlayerController* PC = GetOwningPlayer<AZPlayerController>();
	PC->OnLootingFinished();
}

void UZLootWidget::TakeAll()
{
	const TArray<FZInventoryEntry>& AllItems = ContainerInventory->GetInventoryWidget()->GetInventoryComponent()->GetInventory().Items;
	
	FZLootRequest LootRequest;
	for (const FZInventoryEntry& Item : AllItems)
	{
		LootRequest.NPCItems.Add(Item.ItemId);
	}
	LootRequest.ContainerInventory = ContainerInventory->GetInventoryWidget()->GetInventoryComponent();
	PlayerInventory->GetInventoryWidget()->GetInventoryComponent()->Loot(LootRequest);

	RemoveFromStack();
}

void UZLootWidget::Refresh()
{
	StartLooting(ContainerInventoryComponent);
}

