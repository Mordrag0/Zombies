// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTTaskBase.h"


FZStateTreeAITaskBase::FZStateTreeAITaskBase()
{
	bShouldCallTick = false;
}

#if WITH_EDITOR
FText FZStateTreeAITaskBase::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FString RichLabelStr = GetDescriptionLabel();
	const FText Params = GetDescriptionParams(ID, InstanceDataView, BindingLookup, Formatting);
	return (Formatting == EStateTreeNodeFormatting::RichText)
		? FText::Format(INVTEXT("{0} {1}"), FText::FromString(RichLabelStr), Params)
		: FText::Format(INVTEXT("{0} {1}"), FText::FromString(StripRichTextTags(RichLabelStr)), Params);
}
#endif

FZStateTreeAIActionTaskBase::FZStateTreeAIActionTaskBase()
{
	bShouldCallTick = false;
}

#if WITH_EDITOR
FText FZStateTreeAIActionTaskBase::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FString RichLabelStr = GetDescriptionLabel();
	const FText Params = GetDescriptionParams(ID, InstanceDataView, BindingLookup, Formatting);
	return (Formatting == EStateTreeNodeFormatting::RichText)
		? FText::Format(INVTEXT("{0} {1}"), FText::FromString(RichLabelStr), Params)
		: FText::Format(INVTEXT("{0} {1}"), FText::FromString(StripRichTextTags(RichLabelStr)), Params);
}
#endif

