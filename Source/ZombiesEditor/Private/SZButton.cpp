// Copyright 2026 Luka Markuš. All rights reserved.


#include "SZButton.h"

void SZButton::Construct(const FArguments& InArgs)
{
	OnDoubleClickedDelegate = InArgs._OnDoubleClicked;
	SButton::Construct(SButton::FArguments()
		.ButtonStyle(FAppStyle::Get(), "NoBorder")
		.OnClicked(InArgs._OnClicked)
		.ForegroundColor(InArgs._ForegroundColor)
		.ContentPadding(InArgs._ContentPadding)
		.ButtonColorAndOpacity(InArgs._BackgroundColor)
		.Content()
        [
            InArgs._Content.Widget
        ]
    );
}

FReply SZButton::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if(OnDoubleClickedDelegate.IsBound())
	{
		OnDoubleClickedDelegate.Execute();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

