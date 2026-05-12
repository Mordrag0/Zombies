// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTConditionBase.h"
#include "ZSTCondition_HasCombatTarget.generated.h"

class AZAIController;

USTRUCT()
struct FZSTCondition_HasCombatTargetInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Context")
	TObjectPtr<AZAIController> AIController = nullptr;
};

USTRUCT(meta = (DisplayName = "Has Combat Target?"))
struct FZSTCondition_HasCombatTarget : public FZSTConditionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTCondition_HasCombatTargetInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	STATETREE_NODE_DESCRIPTION()
};
