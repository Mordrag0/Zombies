// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ZCharacterAnimationData.generated.h"

enum class EZCharacterActivity : uint32;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZCharacterAnimationData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMirrorDataTable> MirrorDataTable;

	UPROPERTY(EditDefaultsOnly)
	TMap<EZCharacterActivity, TObjectPtr<UAnimMontage>> ActivityMontages;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> DeathMontage;
};

