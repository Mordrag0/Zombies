// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZSTTask_WaitForPlayer.generated.h"

class AZNPCharacter;
class AZFPCharacter;

USTRUCT()
struct FZSTTask_WaitForPlayerInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AZNPCharacter> Character;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AZFPCharacter> Player;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float Distance = 300.f;
};

USTRUCT(meta = (DisplayName = "Wait for player", Category = "AI|Action"))
struct FZSTTask_WaitForPlayer : public FZStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_WaitForPlayerInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	FZSTTask_WaitForPlayer();

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif

protected:
	bool IsPlayerNear(const FStateTreeExecutionContext& Context) const;
};
