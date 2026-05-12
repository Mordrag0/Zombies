// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZDelegateConnection.h"
#include "ZTypes.h"
#include "ZSTTask_Activity.generated.h"

class AZNPCharacter;
class AZCharacter;
enum class EZCharacterActivityBP : uint8;

USTRUCT()
struct FZSTTask_ActivityInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AZNPCharacter> NPC = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	EZCharacterActivityBP Activity = static_cast<EZCharacterActivityBP>(0);

	FZDelegateConnection OnActivityStoppedConnection;
	FZDelegateConnection OnActivityEndingConnection;
};

USTRUCT(meta = (DisplayName = "Do activity", Category = "AI"))
struct FZSTTask_Activity : public FZStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_ActivityInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

