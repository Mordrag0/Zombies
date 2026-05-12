// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZSTTask_GetBestWeapon.generated.h"

class AZNPCharacter;
class AZWeapon;

USTRUCT()
struct FZSTTask_GetBestWeaponInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AZNPCharacter> Character = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Output")
	TSubclassOf<AZWeapon> WeaponClass = nullptr;
};

USTRUCT(meta = (DisplayName = "Get best weapon", Category = "AI"))
struct FZSTTask_GetBestWeapon : public FZStateTreeAITaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_GetBestWeaponInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	FZSTTask_GetBestWeapon();

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
	STATETREE_NODE_DESCRIPTION()
};
