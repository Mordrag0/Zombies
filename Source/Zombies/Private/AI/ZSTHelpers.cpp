// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZSTHelpers.h"


FString StripRichTextTags(const FString& RichText)
{
	FString Result;
	bool bInsideTag = false;
	for (const TCHAR Char : RichText)
	{
		if (Char == TEXT('<'))
		{
			bInsideTag = true;
		}
		else if (Char == TEXT('>'))
		{
			bInsideTag = false;
		}
		else if (!bInsideTag)
		{
			Result += Char;
		}
	}
	return Result;
}

FText JoinParameters(const TArray<FText>& Params)
{
	if (Params.Num() == 0)
	{
		return FText::GetEmpty();
	}
    FString Result = Params[0].ToString();
    for (int32 Idx = 1; Idx < Params.Num(); ++Idx)
    {
        Result += TEXT(" -> ");
        Result += Params[Idx].ToString();
    }
    return FText::FromString(Result);
}

