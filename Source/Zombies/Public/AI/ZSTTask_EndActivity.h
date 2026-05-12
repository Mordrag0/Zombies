// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZDelegateConnection.h"
#include "ZSTTask_Activity.h"
#include "ZSTTask_EndActivity.generated.h"

class AZNPCharacter;
enum class EZCharacterActivityBP : uint8;

USTRUCT()
struct FZSTTask_EndActivityInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AZNPCharacter> Character;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	EZCharacterActivityBP Activity = static_cast<EZCharacterActivityBP>(0);

	FZDelegateConnection OnActivityStoppedConnection;
};

USTRUCT(meta = (DisplayName = "End activity", Category = "AI|Action"))
struct FZSTTask_EndActivity : public FZStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_EndActivityInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};



