// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "ZDelegateConnection.h"
#include "ZSTTask_Shoot.generated.h"

class AZNPCAIController;
class AZNPCharacter;
class AZRangedWeapon;
class AZCharacter;

USTRUCT()
struct FZSTTask_ShootInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AZNPCharacter> Character;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<AZCharacter> Target;
	
	UPROPERTY()
	TObjectPtr<AZRangedWeapon> Weapon;

	FZDelegateConnection OnOutOfAmmoConnection;

	FZDelegateConnection OnTargetDied;
};

USTRUCT(meta = (DisplayName = "Shoot", Category = "AI|Action"))
struct FZSTTask_Shoot : public FZStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_ShootInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};
