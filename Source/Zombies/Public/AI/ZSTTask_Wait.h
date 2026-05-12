// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZSTTask_Wait.generated.h"

USTRUCT()
struct FZSTTask_WaitInstanceData
{
	GENERATED_BODY()
};

USTRUCT(meta = (DisplayName = "Wait", Category = "AI"))
struct FZSTTask_Wait : public FZStateTreeAITaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_WaitInstanceData;

	virtual const UStruct* GetInstanceDataType() const { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
	STATETREE_NODE_DESCRIPTION()
};
