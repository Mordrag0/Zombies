// Copyright 2026 Luka Markuš. All rights reserved.


#include "Inventory/ZInventoryComponent.h"
#include "Inventory/ZInventoryItem.h"
#include "Characters/ZCharacter.h"
#include "ZTypes.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Engine/AssetManager.h"
#include "Inventory/ZInventoryItemData.h"
#include "Components/ZEquipmentComponent.h"
#include "ZGameInstance.h"
#include "ZLog.h"
#include "UI/ZText.h"

#define LOCTEXT_NAMESPACE "Zombies"

void FZInventoryEntry::PostReplicatedAdd(const FZReplicatedInventory& InArraySerializer)
{
	// Load item asset
	LoadHandle = UAssetManager::Get().LoadPrimaryAsset(ItemId, TArray<FName>({ "Game", "UI" }), // #ZTODO do we need to load this here? Maybe only for the local player?
        FStreamableDelegate::CreateLambda([this, &InArraySerializer]()
        {
            ItemData = Cast<UZInventoryItemData>(UAssetManager::Get().GetPrimaryAssetObject(ItemId));
            if (AZCharacter* Character = InArraySerializer.CharacterOwner.Get())
            {
				Character->GetInventoryComponent()->GetInventoryMap().Add(ItemId, Count);
                Character->GetInventoryComponent()->OnInventoryChanged.Broadcast();
            }
        }));
}

void FZInventoryEntry::PostReplicatedChange(const FZReplicatedInventory& InArraySerializer)
{
	if (AZCharacter* Character = InArraySerializer.CharacterOwner.Get())
	{
		Character->GetInventoryComponent()->GetInventoryMap()[ItemId] = Count;
		Character->GetInventoryComponent()->OnInventoryChanged.Broadcast();
	}
}

void FZInventoryEntry::PreReplicatedRemove(const FZReplicatedInventory& InArraySerializer)
{
	// Unload item asset
	if (LoadHandle.IsValid())
	{
		LoadHandle->CancelHandle(); // Explicitly cancel callback
		LoadHandle.Reset();
	}
	ItemData = nullptr;

	if (AZCharacter* Character = InArraySerializer.CharacterOwner.Get())
	{
		Character->GetInventoryComponent()->GetInventoryMap().Remove(ItemId);
		Character->GetInventoryComponent()->OnInventoryChanged.Broadcast();
	}
}

UZInventoryComponent::UZInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}

void UZInventoryComponent::AddItem(FPrimaryAssetId ItemId, int32 Count, bool bNotifyListeners)
{
	if (!ensure(Count > 0))
	{
		return;
	}
	UAssetManager& Manager = UAssetManager::Get();
	UZInventoryItemData* ItemData = Cast<UZInventoryItemData>(Manager.GetPrimaryAssetObject(ItemId)); // #ZTODO: what if it's not loaded?
	if (!GetOwner()->HasAuthority())
	{
		Server_Reliable_AddItem(ItemId, Count);
		return;
	}
		
	for (FZInventoryEntry& Entry : Inventory.Items)
	{
		if (Entry.ItemId == ItemId)
		{
			Entry.Count += Count;
			Inventory.MarkItemDirty(Entry);
			InventoryMap[ItemId] += Count;

			if (bNotifyListeners)
			{
				OnInventoryChanged.Broadcast();
			}
			return;
		}
	}

	FZInventoryEntry& NewEntry = Inventory.Items.AddDefaulted_GetRef();
	NewEntry.ItemId = ItemId;
	NewEntry.ItemData = ItemData;
	NewEntry.Count = Count;
	NewEntry.LoadHandle = UAssetManager::Get().LoadPrimaryAsset(ItemId, TArray<FName>({ "Game", "UI" }), FStreamableDelegate()); // #ZTODO: bundles and delegate
	InventoryMap.Add(TPair<FPrimaryAssetId, int32>(ItemId, Count));

	// Load and keep loaded

	Inventory.MarkItemDirty(NewEntry);
	if (bNotifyListeners)
	{
		OnInventoryChanged.Broadcast();
	}
}

void UZInventoryComponent::Server_Reliable_AddItem_Implementation(FPrimaryAssetId ItemId, int32 Count)
{
	AddItem(ItemId, Count, true);
}

void UZInventoryComponent::RemoveItem(FPrimaryAssetId ItemId, int32 Count, bool bNotifyListeners)
{
	if (!ensure(Count > 0))
	{
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		Server_Reliable_RemoveItem(ItemId, Count);
		return;
	}

	UAssetManager& Manager = UAssetManager::Get();
	for (int32 Idx = 0; Idx < Inventory.Items.Num(); ++Idx)
	{
		FZInventoryEntry& Entry = Inventory.Items[Idx];

		if (Entry.ItemId == ItemId)
		{
			ensure(Entry.Count >= Count);
			Entry.Count -= Count;
			InventoryMap[ItemId] -= Count;
			if (Entry.Count == 0)
			{
				Inventory.Items.RemoveAt(Idx);
				InventoryMap.Remove(ItemId);
				Inventory.MarkArrayDirty();
			}
			else
			{
				Inventory.MarkItemDirty(Entry);
			}
			if (bNotifyListeners)
			{
				OnInventoryChanged.Broadcast();
			}
			return;
		}
	}
	ensure(0);
}

void UZInventoryComponent::Server_Reliable_RemoveItem_Implementation(FPrimaryAssetId ItemId, int32 Count)
{
	RemoveItem(ItemId, Count, true);
}

int32 UZInventoryComponent::GetItemCount(FPrimaryAssetId ItemId) const
{
	if (const int32* Found = InventoryMap.Find(ItemId))
	{
		return *Found;
	}
	return 0;
}

void UZInventoryComponent::Select(FPrimaryAssetId ItemId)
{
	UAssetManager& Manager = UAssetManager::Get();

	if (!InventoryMap.Contains(ItemId))
	{
		UE_LOG(LogZombies, Error, TEXT("Trying to select an invalid inventory item."));
		return;
	}
	const UZInventoryItemData* ItemData = Cast<UZInventoryItemData>(Manager.GetPrimaryAssetObject(ItemId));
	const AZInventoryItem* ItemCDO = ItemData->ItemClass->GetDefaultObject<AZInventoryItem>();
	if (ItemCDO->CanBeEquipped())
	{
		TSubclassOf<AZEquippableItem> EquippableClass = ItemData->ItemClass.Get();
		if (AZCharacter* CharacterOwner = GetOwner<AZCharacter>())
		{
			CharacterOwner->GetEquipmentComponent()->StartEquip(EquippableClass);
		}
	}
	else if (ItemCDO->CanBeConsumed())
	{

	}
}

int32 UZInventoryComponent::GetCoins() const
{
	UZGameInstance* GI = GetWorld()->GetGameInstance<UZGameInstance>();
	if (!ensure(GI))
	{
		return 0;
	}
	if (const int32* CoinCount = InventoryMap.Find(GI->GetCoinsID()))
	{
		return *CoinCount;
	}
	return 0;
}

void UZInventoryComponent::AddCoins(int32 Amount)
{
	UZGameInstance* GI = GetWorld()->GetGameInstance<UZGameInstance>();
	if (!ensure(GI))
	{
		return;
	}
	int32& CoinCount = InventoryMap.FindOrAdd(GI->GetCoinsID());
	CoinCount += Amount;
}

void UZInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;

    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Inventory, Params);
}

bool UZInventoryComponent::AcceptTrade(const FZTradeRequest& TradeRequest, bool bNotifyPlayer, bool bRemoveItems)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_Reliable_AcceptTrade(TradeRequest, bNotifyPlayer, bRemoveItems);
		return true;
	}
	if (bRemoveItems && !ValidateTrade(TradeRequest))
	{
		return false;
	}
	for (const FZItemStack& Item : TradeRequest.PlayerItems)
	{
		if (bRemoveItems)
		{
			RemoveItem(Item.ID, Item.Count, false);
		}
		TradeRequest.NPCInventory->AddItem(Item.ID, Item.Count, false);
	}
	for (const FZItemStack& Item : TradeRequest.NPCItems)
	{
		if (bRemoveItems)
		{
			TradeRequest.NPCInventory->RemoveItem(Item.ID, Item.Count, false);
		}
		AddItem(Item.ID, Item.Count, false);
	}

	if (TradeRequest.Cost > 0 || bRemoveItems)
	{
		AddCoins(TradeRequest.Cost);
	}
	if (TradeRequest.Cost < 0 || bRemoveItems)
	{
		TradeRequest.NPCInventory->AddCoins(-TradeRequest.Cost);
	}

	OnInventoryChanged.Broadcast();

	if (bNotifyPlayer)
	{
		NotifyPlayerOfTradedItems(TradeRequest);
	}

	return true;
}

void UZInventoryComponent::Server_Reliable_AcceptTrade_Implementation(const FZTradeRequest& TradeRequest, bool bNotifyPlayer, bool bRemoveItems)
{
	AcceptTrade(TradeRequest, bNotifyPlayer, bRemoveItems);
}

void UZInventoryComponent::Loot(const FZLootRequest& LootRequest)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_Reliable_Loot(LootRequest);
		return;
	}
	if (!ValidateLoot(LootRequest))
	{
		return;
	}
	for (FPrimaryAssetId ItemId : LootRequest.NPCItems)
	{
		UAssetManager& Manager = UAssetManager::Get();
		const int32 Count = LootRequest.ContainerInventory->GetItemCount(ItemId);
		LootRequest.ContainerInventory->RemoveItem(ItemId, Count, false);
		AddItem(ItemId, Count, false);
	}
}

void UZInventoryComponent::Server_Reliable_Loot_Implementation(const FZLootRequest& LootRequest)
{
	Loot(LootRequest);
}

void UZInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Inventory.CharacterOwner = GetOwner<AZCharacter>();

	if (GetOwner()->HasAuthority())
	{
		if (DefaultInventoryMap.Num() > 0)
		{
			UAssetManager& Manager = UAssetManager::Get();
			for (const TPair<FPrimaryAssetId, int32>& DefaultItem : DefaultInventoryMap)
			{
				UZInventoryItemData* ItemData = Cast<UZInventoryItemData>(Manager.GetPrimaryAssetObject(DefaultItem.Key));
				FZInventoryEntry& NewEntry = Inventory.Items.AddDefaulted_GetRef();
				NewEntry.ItemId = DefaultItem.Key;
				NewEntry.ItemData = ItemData;
				NewEntry.Count = DefaultItem.Value;
				NewEntry.LoadHandle = UAssetManager::Get().LoadPrimaryAsset(DefaultItem.Key, TArray<FName>({ "Game", "UI" }), FStreamableDelegate()); // #ZTODO: bundles and delegate
				InventoryMap.Add(TPair<FPrimaryAssetId, int32>(DefaultItem.Key, DefaultItem.Value));

				OnInventoryChanged.Broadcast();
			}
			Inventory.MarkArrayDirty();
		}
	}
}

bool UZInventoryComponent::ValidateTrade(const FZTradeRequest& TradeRequest) const
{
	for (const FZItemStack& Item : TradeRequest.PlayerItems)
	{
		if (!InventoryMap.Contains(Item.ID) || InventoryMap[Item.ID] < Item.Count) // #ZTODO: simplify to one function call
		{
			return false;
		}
	}
	for (const FZItemStack& Item : TradeRequest.NPCItems)
	{
		const TMap<FPrimaryAssetId, int32>& NPCInventoryMap = TradeRequest.NPCInventory->GetInventoryMap();
		if (!NPCInventoryMap.Contains(Item.ID) || NPCInventoryMap[Item.ID] < Item.Count)
		{
			return false;
		}
	}
	if (TradeRequest.Cost > 0) // If cost of the trade > 0 then we're taking money out of NPCs inventory
	{
		if (TradeRequest.NPCInventory->GetCoins() < TradeRequest.Cost)
		{
			return false;
		}
	}
	else // else we take money out of player inventory
	{
		if (GetCoins() < FMath::Abs(TradeRequest.Cost))
		{
			return false;
		}
	}
	return true;
}

bool UZInventoryComponent::ValidateLoot(const FZLootRequest& LootRequest) const
{
	UAssetManager& Manager = UAssetManager::Get();
	
	for (FPrimaryAssetId ItemId : LootRequest.NPCItems)
	{
		const TMap<FPrimaryAssetId, int32>& NPCInventoryMap = LootRequest.ContainerInventory->GetInventoryMap();
		if (!NPCInventoryMap.Contains(ItemId))
		{
			return false;
		}
	}
	return true;
}

void UZInventoryComponent::NotifyPlayerOfTradedItems(const FZTradeRequest& TradeRequest)
{
	if (!GetOwner()->HasLocalNetOwner())
	{
		Client_Reliable_NotifyPlayerOfTradedItems(TradeRequest);
		return;
	}
	APawn* PawnOwner = GetOwner<APawn>();
	AZPlayerController* PC = PawnOwner ? PawnOwner->GetController<AZPlayerController>() : nullptr;
	if (!ensure(PC))
	{
		return;
	}
	
	const UZGameInstance* GI = GetWorld()->GetGameInstance<UZGameInstance>();
	const UAssetManager& Manager = UAssetManager::Get();
	const UZInventoryItemData* CoinsData = Cast<UZInventoryItemData>(Manager.GetPrimaryAssetObject(GI->GetCoinsID()));
	if (TradeRequest.Cost > 0)
	{
		PC->AddNotification(EZNotificationType::ItemReceived, FText::Format(ZText::ItemReceived, CoinsData->DisplayName, TradeRequest.Cost));
	}
	else if (TradeRequest.Cost < 0) // No notification if Cost == 0
	{
		PC->AddNotification(EZNotificationType::ItemGiven, FText::Format(ZText::ItemGiven, CoinsData->DisplayName, FMath::Abs(TradeRequest.Cost)));
	}
	for (const FZItemStack& ItemStack : TradeRequest.PlayerItems)
	{
		const UZInventoryItemData* ItemData = Cast<UZInventoryItemData>(Manager.GetPrimaryAssetObject(ItemStack.ID));
		PC->AddNotification(EZNotificationType::ItemGiven, FText::Format(ZText::ItemGiven, ItemData->DisplayName, ItemStack.Count));
	}
	for (const FZItemStack& ItemStack : TradeRequest.NPCItems)
	{
		const UZInventoryItemData* ItemData = Cast<UZInventoryItemData>(Manager.GetPrimaryAssetObject(ItemStack.ID));
		PC->AddNotification(EZNotificationType::ItemReceived, FText::Format(ZText::ItemReceived, ItemData->DisplayName, ItemStack.Count));
	}
}

void UZInventoryComponent::Client_Reliable_NotifyPlayerOfTradedItems_Implementation(const FZTradeRequest& TradeRequest)
{
	NotifyPlayerOfTradedItems(TradeRequest);
}

#undef LOCTEXT_NAMESPACE

