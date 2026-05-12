// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTConditionBase.h"
#include "ZSTCondition_CanReload.generated.h"

class AZNPCharacter;


USTRUCT()
struct FZSTCondition_CanReloadInstanceData
{
	GENERATED_BODY()
	
    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<AZNPCharacter> NPC;
};

USTRUCT(meta = (DisplayName = "Can reload?"))
struct FZSTCondition_CanReload: public FZSTInvertableCondition
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTCondition_CanReloadInstanceData;

	FZSTCondition_CanReload() {}

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	STATETREE_NODE_DESCRIPTION()
};
