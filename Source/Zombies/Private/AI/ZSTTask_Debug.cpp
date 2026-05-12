// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTask_Debug.h"
#include "StateTreeExecutionContext.h"
#include "Characters/ZNPCharacter.h"
#include "ZTypes.h"
#include "Logging/LogVerbosity.h"
#include "ZInteractable.h"
#include "StateTreeTypes.h"

DEFINE_LOG_CATEGORY(LogZStateTree)

FZSTTask_Debug::FZSTTask_Debug()
{
	bShouldCallTick = true;
}

EStateTreeRunStatus FZSTTask_Debug::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	Log(TEXT("ENTER"), Context);
	if (bLogTransition)
	{
		LogTransition(Context, Transition);
	}
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FZSTTask_Debug::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (bTick)
	{
		Log(TEXT("TICK"), Context);
	}
	return EStateTreeRunStatus::Running;
}

void FZSTTask_Debug::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (bTick || bLogTransition)
	{
		Log(TEXT("EXIT"), Context);
	}
	if (bLogTransition)
	{
		LogTransition(Context, Transition);
	}
}

#if WITH_EDITOR
FText FZSTTask_Debug::GetDescriptionParams(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);
	return InstanceData->Character ? FText::FromName(InstanceData->Character->GetNPCName()) : FText::GetEmpty();
}
#endif

void FZSTTask_Debug::Log(const FString& State, const FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character)
	{
		return;
	}
	if (!InstanceData.NPCName.IsEmpty() && !InstanceData.Character->GetNPCName().ToString().Contains(InstanceData.NPCName))
	{
		return;
	}
	const ELogVerbosity::Type LogVerbosity = GetLogVerbosity(InstanceData.LogVerbosity);
	FMsg::Logf(__FILE__, __LINE__, LogZStateTree.GetCategoryName(), LogVerbosity, 
		TEXT("%s %s -> %s"), *State, *InstanceData.Character->GetNPCName().ToString(), *GetString(Context));
}

FString FZSTTask_Debug::GetString(const FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (FAISystem::IsValidLocation(InstanceData.Location))
	{
		if (DrawDuration > 0.f)
		{
			DrawDebugSphere(Context.GetWorld(), InstanceData.Location, 30.f, 30, DrawColor, false, DrawDuration, 0, 2.f);
		}
		return InstanceData.Location.ToString();
	}
	if (InstanceData.Object)
	{
		return InstanceData.Object->GetName();
	}
	if (InstanceData.Interactable && InstanceData.Interactable.GetObject())
	{
		return InstanceData.Interactable.GetObject()->GetName();
	}
	if (UEnum* Enum = FindObject<UEnum>(nullptr, *FString::Printf(TEXT("/Script/Zombies.%s"), *InstanceData.EnumTypeName)))
	{
		return Enum->GetNameStringByValue(static_cast<int64>(InstanceData.EnumValue));
	}
	if (!InstanceData.Text.IsEmpty())
	{
		return InstanceData.Text;
	}
	if (InstanceData.bPrintNumber)
	{
		return FString::FromInt(InstanceData.Number);
	}

	TStringBuilder<2048> DebugString;
	for (int32 Idx = 0; Idx < Context.GetActiveFrames().Num(); ++Idx)
	{
		for (const FName& StateName : Context.GetActiveStateNames())
		{
			DebugString << StateName;
			DebugString << " => ";
		}
	}
	DebugString.RemoveAt(DebugString.Len() - 4, 4);

	return DebugString.ToString();
}

void FZSTTask_Debug::LogTransition(const FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character)
	{
		return;
	}
	if (!InstanceData.NPCName.IsEmpty() && !InstanceData.Character->GetNPCName().ToString().Contains(InstanceData.NPCName))
	{
		return;
	}
	const ELogVerbosity::Type LogVerbosity = GetLogVerbosity(InstanceData.LogVerbosity);

	TStringBuilder<2048> DebugString;
	TNotNull<const UStateTree*> StateTree = Context.GetStateTree();
	TConstArrayView<FCompactStateTreeState> CompactStates = StateTree->GetStates();
	
	// if (Transition.SourceState.IsValid() && CompactStates.IsValidIndex(Transition.SourceState.Index))
	// {
	// 	const FCompactStateTreeState& SourceState = CompactStates[Transition.SourceState.Index];
	// 	DebugString << "FROM: ";
	// 	DebugString << SourceState.Name.ToString();
	// }
	if (Transition.TargetState.IsValid() && CompactStates.IsValidIndex(Transition.TargetState.Index))
	{
		const FCompactStateTreeState& TargetState = CompactStates[Transition.TargetState.Index];
		DebugString << "TO: ";
		DebugString << TargetState.Name.ToString();
	}
	const FString Str = DebugString.ToString();
	FMsg::Logf(__FILE__, __LINE__, LogZStateTree.GetCategoryName(), LogVerbosity,
		TEXT("%s -> %s"), *InstanceData.Character->GetNPCName().ToString(), *Str);
}

ELogVerbosity::Type FZSTTask_Debug::GetLogVerbosity(EZSTLogVerbosity Verbosity) const
{
	switch (Verbosity)
    {
		case EZSTLogVerbosity::VeryVerbose: return ELogVerbosity::VeryVerbose;
		case EZSTLogVerbosity::Verbose: return ELogVerbosity::Verbose;
		case EZSTLogVerbosity::Log: return ELogVerbosity::Log;
        case EZSTLogVerbosity::Warning: return ELogVerbosity::Warning;
		case EZSTLogVerbosity::Error: return ELogVerbosity::Error;
    }
	return ELogVerbosity::Log;
}
