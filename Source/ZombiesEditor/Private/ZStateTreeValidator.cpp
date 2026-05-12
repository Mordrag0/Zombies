// Copyright 2026 Luka Markuš. All rights reserved.


#include "ZStateTreeValidator.h"
#include "StateTree.h"
#include "StateTreeEditorData.h"
#include "StructUtils/PropertyBag.h"
#include "StateTreePropertyBindings.h"
#include "PropertyBindingBindingCollection.h"
#include "PropertyBindingPath.h"
#include "StateTreeTypes.h"
#include "Misc/DataValidation.h"

bool UZStateTreeValidator::CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const
{
	return InObject->IsA<UStateTree>();
}

EDataValidationResult UZStateTreeValidator::ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context)
{
	const UStateTree* StateTree = Cast<UStateTree>(InAsset);

	if (!StateTree)
	{
		return EDataValidationResult::NotValidated;
	}
	UStateTreeEditorData* TreeData = Cast<UStateTreeEditorData>(StateTree->EditorData);
	if (!TreeData)
	{
		return EDataValidationResult::NotValidated;
	}

	TreeData->VisitHierarchy(
		[&Context, &TreeData, &InAssetData](UStateTreeState& State, UStateTreeState* /*ParentState*/)
	{
		if (State.Type == EStateTreeStateType::LinkedAsset && State.LinkedAsset)
		{
			const FGuid& ParamStructID = State.Parameters.ID;
			if (const UScriptStruct* ScriptStruct = State.Parameters.Parameters.GetPropertyBagStruct())
			{
				for (TFieldIterator<FProperty> It(ScriptStruct); It; ++It)
				{
					FProperty* Property = *It;
					bool bFound = false;
					TreeData->GetEditorPropertyBindings()->VisitBindings([&bFound, Property, &ParamStructID](const FPropertyBindingBinding& Binding)
					{
						TConstArrayView<FPropertyBindingPathSegment> Segments = Binding.GetTargetPath().GetSegments();
						if ((Segments.Num() > 0) 
							&& (Segments.Last().GetName() == Property->GetFName())
							&& Binding.GetTargetPath().GetStructID() == ParamStructID)
						{
							bFound = true;
							return FPropertyBindingBindingCollection::EVisitResult::Break;
						}
						return FPropertyBindingBindingCollection::EVisitResult::Continue;
					});
					if (!bFound)
					{
						Context.AddMessage(InAssetData, EMessageSeverity::Error, FText::Format(
							INVTEXT("Linked asset '{0}' parameter '{1}' is not bound"),
							FText::FromName(State.Name),
							FText::FromName(Property->GetFName())));
					}
				}
			}
		}
		return EStateTreeVisitor::Continue;
	});
	return EDataValidationResult::Valid;
}

