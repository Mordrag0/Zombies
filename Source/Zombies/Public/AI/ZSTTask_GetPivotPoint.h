// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZSTTask_GetPivotPoint.generated.h"

class IZInteractable;

USTRUCT()
struct FZSTTask_GetPivotPointInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Input")
	TScriptInterface<IZInteractable> Interactable;

	UPROPERTY(VisibleAnywhere, Category = "Output")
	FTransform PivotPoint;
};

USTRUCT(meta = (DisplayName = "Get pivot point", Category = "AI"))
struct FZSTTask_GetPivotPoint : public FZStateTreeAITaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_GetPivotPointInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	FZSTTask_GetPivotPoint();

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};
