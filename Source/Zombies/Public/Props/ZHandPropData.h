// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ZHandPropData.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZHandPropData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UStaticMesh> Mesh = nullptr;

    UPROPERTY(EditDefaultsOnly)
    FName SocketName = NAME_None;

    UPROPERTY(EditDefaultsOnly)
    FVector RelativeLocation = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly)
    FRotator RelativeRotation = FRotator::ZeroRotator;

    UPROPERTY(EditDefaultsOnly)
    FVector RelativeScale = FVector::OneVector;

    // Tells the asset manager what type this is
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId("HandProp", GetFName());
    }
};

