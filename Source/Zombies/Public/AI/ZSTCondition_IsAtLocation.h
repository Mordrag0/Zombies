// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTConditionBase.h"
#include "AITypes.h"
#include "ZSTCondition_IsAtLocation.generated.h"

class AZNPCharacter;

USTRUCT()
struct FZSTCondition_IsAtLocationInstanceData
{
	GENERATED_BODY()
	
    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<AZNPCharacter> Character;

	UPROPERTY(EditAnywhere, Category = "Input")
	FVector Location = FAISystem::InvalidLocation;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (Optional))
	FRotator Rotation = FAISystem::InvalidRotation;
};

USTRUCT(meta = (DisplayName = "Is at location?"))
struct FZSTCondition_IsAtLocation : public FZSTInvertableCondition
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTCondition_IsAtLocationInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	
	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif

	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bCheckRotation = true;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float DistanceTolerance = 5.f;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AngleTolerance = 10.f;
};


