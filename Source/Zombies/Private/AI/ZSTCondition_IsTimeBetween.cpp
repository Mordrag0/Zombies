// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTCondition_IsTimeBetween.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeNodeDescriptionHelpers.h"
#include "ZGameplayStatics.h"


bool FZSTCondition_IsTimeBetween::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	const bool bResult = UZGameplayStatics::IsHourBetween(InstanceData.NewHour, InstanceData.FromHour, InstanceData.ToHour);
	return bResult ^ bInvert;
}

#if WITH_EDITOR
FText FZSTCondition_IsTimeBetween::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FText InvertText = UE::StateTree::DescHelpers::GetInvertText(bInvert, Formatting);
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	const FText From = GET_NUMBER_DISPLAY_TEXT(FromHour);
	const FText To = GET_NUMBER_DISPLAY_TEXT(ToHour);
	const FText Hour = GET_NUMBER_DISPLAY_TEXT(NewHour);

	const FString FormatStr = TEXT("<s>{0}Is time</> {1} <s>between</> {2} <s>and</> {3}");
	const FText FormatText = Formatting == EStateTreeNodeFormatting::RichText
		? FText::AsCultureInvariant(FormatStr)
		: FText::AsCultureInvariant(StripRichTextTags(FormatStr));

	return FText::Format(FormatText, InvertText, Hour, From, To);
}
#endif

