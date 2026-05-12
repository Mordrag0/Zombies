// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTConditionBase.h"
#include "StateTreeNodeDescriptionHelpers.h"


#if WITH_EDITOR

FText FZSTConditionBase::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FString RichLabelStr = GetDescriptionLabel();
	const FText Params = GetDescriptionParams(ID, InstanceDataView, BindingLookup, Formatting);
	return (Formatting == EStateTreeNodeFormatting::RichText)
		? FText::Format(INVTEXT("{0} {1}"), FText::FromString(RichLabelStr), Params)
		: FText::Format(INVTEXT("{0} {1}"), FText::FromString(StripRichTextTags(RichLabelStr)), Params);
}
#endif

#if WITH_EDITOR
FText FZSTInvertableCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FText InvertText = UE::StateTree::DescHelpers::GetInvertText(bInvert, Formatting);
	return FText::Format(INVTEXT("{0}{1}"), InvertText, Super::GetDescription(ID, InstanceDataView, BindingLookup, Formatting));
}
#endif

