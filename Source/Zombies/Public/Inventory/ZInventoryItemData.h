// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ZInventoryItemData.generated.h"

class AZInventoryItem;
class UTexture2D;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZInventoryItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AssetBundles = "Game"))
    TSoftClassPtr<AZInventoryItem> ItemClass;

    UPROPERTY(EditDefaultsOnly)
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, meta = (AssetBundles = "UI"))
    TSoftObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditDefaultsOnly)
    uint32 Value;

    UPROPERTY(EditDefaultsOnly)
    bool bHideInTradeUI;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId("Item", GetFName());
    }
};

