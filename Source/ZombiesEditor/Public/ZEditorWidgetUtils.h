// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"

namespace ZEditorWidgetUtils
{
	TSharedRef<SWidget> BuildButton(const FText& ButtonText, TFunction<void()> OnClicked);
	void EditorSelectActor(AActor* Actor);
}

