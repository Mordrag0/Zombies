// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZDelegateConnection.h"
#include "ZSTTask_Reload.generated.h"

class AZNPCharacter;
struct FStateTreeWeakExecutionContext;

USTRUCT()
struct FZSTTask_ReloadInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AZNPCharacter> Character;

	FZDelegateConnection OnWaitForIdleConnection;
	FZDelegateConnection OnReloadCompleteConnection;
};

USTRUCT(meta = (DisplayName = "Reload", Category = "AI"))
struct FZSTTask_Reload : public FZStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_ReloadInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	STATETREE_NODE_DESCRIPTION()

protected:
	void Reload(FStateTreeWeakExecutionContext WeakContext) const;
};

