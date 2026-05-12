// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTConditionBase.h"
#include "ZSTCondition_IsEscortingPlayer.generated.h"

class AZNPCAIController;

USTRUCT()
struct FZSTCondition_IsEscortingPlayerInstanceData
{
	GENERATED_BODY()
	
    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<AZNPCAIController> Controller;
};

USTRUCT(meta = (DisplayName = "Is escorting player?"))
struct FZSTCondition_IsEscortingPlayer : public FZSTInvertableCondition
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTCondition_IsEscortingPlayerInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	
	STATETREE_NODE_DESCRIPTION()
};



