// Copyright 2026 Luka Markuš. All rights reserved.


#include "ZEditorWidgetUtils.h"


TSharedRef<SWidget> ZEditorWidgetUtils::BuildButton(const FText& ButtonText, TFunction<void()> OnClicked)
{
	return SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Border"))
			.BorderBackgroundColor(FLinearColor::White)
			[
				SNew(SButton)
				.ContentPadding(FMargin(6.f, 3.f))
				.ButtonColorAndOpacity(FLinearColor::Black)
				.OnClicked_Lambda([OnClicked]() { OnClicked(); return FReply::Handled(); })
				[
					SNew(STextBlock)
					.Text(ButtonText)
				]
			];
}

void ZEditorWidgetUtils::EditorSelectActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	GEditor->MoveViewportCamerasToActor(*Actor, false);
	GEditor->SelectNone(true, true);
	GEditor->SelectActor(Actor, true, true, true);
	GEditor->NoteSelectionChange();
}

