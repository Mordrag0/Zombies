// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "AI/ZSTHelpers.h"
#include "ZSTConditionBase.generated.h"

USTRUCT(meta = (Category = "AI", Hidden))
struct FZSTConditionBase : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()
	
#if WITH_EDITOR
	virtual FString GetDescriptionLabel() const { unimplemented(); return TEXT(""); }
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const { return FText::GetEmpty(); }
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

USTRUCT(meta = (Hidden))
struct FZSTInvertableCondition : public FZSTConditionBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInvert = false;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
