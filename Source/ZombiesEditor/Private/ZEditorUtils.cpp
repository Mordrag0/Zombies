// Copyright 2026 Luka Markuš. All rights reserved.


#include "ZEditorUtils.h"
#include "StateTree.h"
#include "StateTreeEditorData.h"

TArray<FString> UZEditorUtils::FindStatesWithNode(UStateTree* StateTree, const FString& TaskName)
{
	TArray<FString> Results;
	if (!StateTree) 
	{
		return Results;
	}

	UStateTreeEditorData* EditorData = Cast<UStateTreeEditorData>(StateTree->EditorData);
	if (!EditorData) 
	{
		return Results;
	}

	TFunction<void(UStateTreeState*)> Search = [&](UStateTreeState* State)
	{
		for (const FStateTreeEditorNode& Node : State->Tasks)
		{
			if (Node.Node.GetScriptStruct() && Node.Node.GetScriptStruct()->GetName().Contains(TaskName))
			{
				Results.Add(State->Name.ToString());
			}
		}
		for (const FStateTreeEditorNode& Node : State->EnterConditions)
		{
			if (Node.Node.GetScriptStruct() && Node.Node.GetScriptStruct()->GetName().Contains(TaskName))
			{
				Results.Add(State->Name.ToString());
			}
		}
		for (const FStateTreeTransition& Transition : State->Transitions)
		{
			for (const FStateTreeEditorNode& Node : Transition.Conditions)
			{
				if (Node.Node.GetScriptStruct() && Node.Node.GetScriptStruct()->GetName().Contains(TaskName))
				{
					Results.Add(State->Name.ToString());
				}
			}
		}
		for (UStateTreeState* Child : State->Children)
		{
			Search(Child);
		}
	};

	for (UStateTreeState* SubTree : EditorData->SubTrees)
	{
		Search(SubTree);
	}

	return Results;
}

