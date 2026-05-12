// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZSTTask_GetAssignedInteractable.generated.h"

class IZInteractable;
class AZNPCharacter;
enum class EZCharacterActivityBP : uint8;

USTRUCT()
struct FZSTTask_GetAssignedInteractableInstanceData
{
	GENERATED_BODY()

	FZSTTask_GetAssignedInteractableInstanceData();
	
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AZNPCharacter> Character = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	EZCharacterActivityBP Activity;

	UPROPERTY(VisibleAnywhere, Category = "Output")
	TScriptInterface<IZInteractable> Interactable = nullptr;
};

USTRUCT(meta = (DisplayName = "Get assigned interactable", Category = "AI"))
struct FZSTTask_GetAssignedInteractable : public FZStateTreeAITaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_GetAssignedInteractableInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	FZSTTask_GetAssignedInteractable();

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};
