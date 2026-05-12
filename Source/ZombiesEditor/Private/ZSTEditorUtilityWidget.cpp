// Copyright 2026 Luka Markuš All rights reserved.


#include "ZSTEditorUtilityWidget.h"
#include "EditorUtilityWidgetComponents.h"
#include "StateTree.h"
#include "StateTreeEditorData.h"
#include "StateTreeState.h"
#include "StateTreeTypes.h"


void UZSTEditorUtilityWidget::NativeConstruct()
{
	Super::NativeConstruct();
	TestButton->OnClicked.AddDynamic(this, &ThisClass::Test);
}

void UZSTEditorUtilityWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UZSTEditorUtilityWidget::Test() const
{
	if (!AsStateTree)
	{
		return;
	}
	
	UStateTreeEditorData* EditorData = Cast<UStateTreeEditorData>(AsStateTree->EditorData);
	if (!EditorData) 
	{
		return;
	}

	TFunction<void(UStateTreeState*)> Search = [&](UStateTreeState* State)
	{
		for (const FStateTreeTransition& Transition : State->Transitions)
		{
			for (const FStateTreeEditorNode& Node : Transition.Conditions)
			{
				if (Transition.RequiredEvent.Tag.ToString().Contains("TargetChanged"))
				{
					UE_LOG(LogTemp, Error, TEXT("Requrid event invalid"));
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
}

