// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/StateTreeAITask.h"
#include "AI/ZSTHelpers.h"
#include "ZSTTaskBase.generated.h"

// Base class of all AI task that expect to be run on an AIController or derived class
USTRUCT(meta = (Hidden))
struct FZStateTreeAITaskBase : public FStateTreeAITaskBase
{
	GENERATED_BODY()

	FZStateTreeAITaskBase();

#if WITH_EDITOR
	virtual FString GetDescriptionLabel() const { unimplemented(); return TEXT(""); }
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const { return FText::GetEmpty(); }
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

// Base class of all AI task that do a physical action 
USTRUCT(meta = (Hidden))
struct FZStateTreeAIActionTaskBase : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	FZStateTreeAIActionTaskBase();

#if WITH_EDITOR
	virtual FString GetDescriptionLabel() const { unimplemented(); return TEXT(""); }
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const { return FText::GetEmpty(); }
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
