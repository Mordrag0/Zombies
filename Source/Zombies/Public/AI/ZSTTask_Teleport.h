// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZBTTask_GetWaypointLocation.h"
#include "ZSTTaskBase.h"
#include "ZSTTask_Teleport.generated.h"

class AZNPCharacter;

USTRUCT()
struct FZSTTask_TeleportInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Context")
	TObjectPtr<AZNPCharacter> Character;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	FVector TargetLocation = FAISystem::InvalidLocation;
};

USTRUCT(meta = (DisplayName = "Teleport to", Category = "AI|Action"))
struct FZSTTask_Teleport : public FZStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_TeleportInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};
