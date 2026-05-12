// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZSTTaskBase.h"
#include "StateTreeAnyEnum.h"
#include "AITypes.h"
#include "ZSTTask_Debug.generated.h"

class AZNPCharacter;
class IZInteractable;
enum class EZCharacterActivityBP : uint8;

DECLARE_LOG_CATEGORY_EXTERN(LogZStateTree, Log, All);

UENUM()
enum class EZSTLogVerbosity : uint8
{
	VeryVerbose,
	Verbose,
    Log,
    Warning,
    Error
};

USTRUCT()
struct FZSTTask_DebugInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Context")
	TObjectPtr<AZNPCharacter> Character;

	UPROPERTY(EditAnywhere, Category = "Parameter") 
	FString NPCName;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	EZSTLogVerbosity LogVerbosity = EZSTLogVerbosity::Log;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FVector Location = FAISystem::InvalidLocation;

	UPROPERTY(EditAnywhere, Category = "Input", meta=(Optional, AllowAnyBinding))
	TObjectPtr<UObject> Object;

	UPROPERTY(EditAnywhere, Category = "Input", meta=(Optional))
	TScriptInterface<IZInteractable> Interactable;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FString EnumTypeName;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (AllowAnyBinding))
	uint8 EnumValue = 0;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FString Text;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bPrintNumber = false;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	int32 Number = 0;
};

USTRUCT(meta = (DisplayName = "Debug", Category = "AI"))
struct FZSTTask_Debug : public FZStateTreeAITaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZSTTask_DebugInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	FZSTTask_Debug();

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	STATETREE_NODE_DESCRIPTION()

#if WITH_EDITOR
	virtual FText GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif

	void Log(const FString& State, const FStateTreeExecutionContext& Context) const;

	FString GetString(const FStateTreeExecutionContext& Context) const;

	void LogTransition(const FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bTick = false;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bLogTransition = false;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float DrawDuration = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FColor DrawColor = FColor::Red;

private:
	ELogVerbosity::Type GetLogVerbosity(EZSTLogVerbosity Verbosity) const;
};
