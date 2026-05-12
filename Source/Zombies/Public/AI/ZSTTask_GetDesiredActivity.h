// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZSTTask_Activity.h"
#include "ZSTTask_GetDesiredActivity.generated.h"

class AZNPCAIController;
enum class EZCharacterActivityBP : uint8;

USTRUCT()
struct FZSTTask_GetDesiredActivityInstanceData
{
	GENERATED_BODY()

	FZSTTask_GetDesiredActivityInstanceData();
	
	UPROPERTY(VisibleAnywhere, Category = "Context")
	TObjectPtr<AZNPCAIController> Controller;

	UPROPERTY(VisibleAnywhere, Category = "Output")
	EZCharacterActivityBP Activity = static_cast<EZCharacterActivityBP>(0);

	UPROPERTY(VisibleAnywhere, Category = "Output")
	bool bInteractionActivity = false;
};

USTRUCT(meta = (DisplayName = "Get desired activity", Category = "AI"))
struct FZSTTask_GetDesiredActivity : public FZStateTreeAITaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_GetDesiredActivityInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
	STATETREE_NODE_DESCRIPTION()
};

