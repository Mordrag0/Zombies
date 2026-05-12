// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZDelegateConnection.h"
#include "ZSTTask_Activity.h"
#include "ZSTTask_EndInteract.generated.h"

class AZNPCharacter;
class IZInteractable;
enum class EZCharacterActivityBP : uint8;

USTRUCT()
struct FZSTTask_EndInteractInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AZNPCharacter> Character;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	TScriptInterface<IZInteractable> Interactable;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	EZCharacterActivityBP Activity = static_cast<EZCharacterActivityBP>(0);

	FZDelegateConnection OnInteractionEndedConnection;
};

USTRUCT(meta = (DisplayName = "End interact with", Category = "AI|Action"))
struct FZSTTask_EndInteract : public FZStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_EndInteractInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};



