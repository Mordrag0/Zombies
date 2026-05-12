// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "ZInventoryItemData.h"
#include "ZInventoryComponent.generated.h"

struct FStreamableHandle;
class AZCharacter;

DECLARE_MULTICAST_DELEGATE(FZOnInventoryChanged);

USTRUCT()
struct FZInventoryEntry : public FFastArraySerializerItem
{
    GENERATED_BODY()

	FZInventoryEntry() : ItemId(FPrimaryAssetId()), ItemData(nullptr), Count(0) {}

    UPROPERTY()
    FPrimaryAssetId ItemId;
	
    UPROPERTY(NotReplicated)
    TObjectPtr<UZInventoryItemData> ItemData;

    UPROPERTY()
    int32 Count;

	// Not replicated - local only
	TSharedPtr<FStreamableHandle> LoadHandle; // #ZTODO: separate into GameHandle and UIHandle
	
    // Called on client when this entry is added:
    void PostReplicatedAdd(const FZReplicatedInventory& InArraySerializer);
    
    // Called on client when this entry is changed:
    void PostReplicatedChange(const FZReplicatedInventory& InArraySerializer);
    
    // Called on client when this entry is removed:
    void PreReplicatedRemove(const FZReplicatedInventory& InArraySerializer);
};

USTRUCT()
struct FZReplicatedInventory : public FFastArraySerializer
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FZInventoryEntry> Items;

	TWeakObjectPtr<AZCharacter> CharacterOwner;

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
    {
        return FFastArraySerializer::FastArrayDeltaSerialize(Items, DeltaParams, *this);
    }
};

template<>
struct TStructOpsTypeTraits<FZReplicatedInventory> : public TStructOpsTypeTraitsBase2<FZReplicatedInventory>
{
    enum
    {
        WithNetDeltaSerializer = true,
    };
};

USTRUCT()
struct FZItemStack
{
	GENERATED_BODY()

	FZItemStack() : ID(FPrimaryAssetId()), Count(0) {}
	FZItemStack(FPrimaryAssetId InID, int32 InCount) : ID(InID), Count(InCount) {}
	FZItemStack(const TPair<FPrimaryAssetId, int32>& KVP) : ID(KVP.Key), Count(KVP.Value) {}

	UPROPERTY()
	FPrimaryAssetId ID;
	
	UPROPERTY()
	int32 Count;
};

USTRUCT()
struct FZTradeRequest
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FZItemStack> PlayerItems;

	UPROPERTY()
	TArray<FZItemStack> NPCItems;

	UPROPERTY()
	int32 Cost = 0;

	UPROPERTY()
	TObjectPtr<UZInventoryComponent> NPCInventory = nullptr;
};

USTRUCT()
struct FZLootRequest
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FPrimaryAssetId> NPCItems;

	UPROPERTY()
	TObjectPtr<UZInventoryComponent> ContainerInventory;
};

UCLASS(ClassGroup=(Custom))
class ZOMBIES_API UZInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UZInventoryComponent();

	const TMap<FPrimaryAssetId, int32>& GetInventoryMap() const { return InventoryMap; }
	TMap<FPrimaryAssetId, int32>& GetInventoryMap() { return InventoryMap; }

	const FZReplicatedInventory& GetInventory() const { return Inventory; }

	void AddItem(FPrimaryAssetId ItemId, int32 Count, bool bNotifyListeners);

	void RemoveItem(FPrimaryAssetId ItemId, int32 Count, bool bNotifyListeners);

	int32 GetItemCount(FPrimaryAssetId ItemId) const;

	void Select(FPrimaryAssetId ItemClass);

	UFUNCTION(BlueprintPure)
	int32 GetCoins() const;

	void AddCoins(int32 Amount);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool AcceptTrade(const FZTradeRequest& TradeRequest, bool bNotifyPlayer, bool bRemoveItems = true);

	void Loot(const FZLootRequest& LootRequest);

	bool IsEmpty() const { return (Inventory.Items.Num() == 0); }

	FZOnInventoryChanged OnInventoryChanged;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_Reliable_AddItem(FPrimaryAssetId ItemId, int32 Count);

	UFUNCTION(Server, Reliable)
	void Server_Reliable_RemoveItem(FPrimaryAssetId ItemId, int32 Count);

	UFUNCTION(Server, Reliable)
	void Server_Reliable_AcceptTrade(const FZTradeRequest& TradeRequest, bool bNotifyPlayer, bool bRemoveItems);

	UFUNCTION(Server, Reliable)
	void Server_Reliable_Loot(const FZLootRequest& LootRequest);

	bool ValidateTrade(const FZTradeRequest& TradeRequest) const;

	bool ValidateLoot(const FZLootRequest& LootRequest) const;

	void NotifyPlayerOfTradedItems(const FZTradeRequest& TradeRequest);

	UFUNCTION(Client, Reliable)
	void Client_Reliable_NotifyPlayerOfTradedItems(const FZTradeRequest& TradeRequest);

	UPROPERTY(EditAnywhere)
	TMap<FPrimaryAssetId, int32> DefaultInventoryMap;

	UPROPERTY()
	TMap<FPrimaryAssetId, int32> InventoryMap;

	UPROPERTY(Replicated)
	FZReplicatedInventory Inventory;
};


