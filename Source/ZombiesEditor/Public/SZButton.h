// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SButton.h"

class SZButton : public SButton
{
public:
    SLATE_BEGIN_ARGS(SZButton) 
		: _ForegroundColor(FSlateColor::UseForeground())
		, _BackgroundColor(FSlateColor(FLinearColor::Black))
		{}
        SLATE_EVENT(FOnClicked, OnDoubleClicked)
		SLATE_EVENT(FOnClicked, OnClicked)
		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_ATTRIBUTE(FMargin, ContentPadding)
		SLATE_ATTRIBUTE(FSlateColor, ForegroundColor)
        SLATE_ATTRIBUTE(FSlateColor, BackgroundColor)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    virtual FReply OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    FOnClicked OnDoubleClickedDelegate;
};

