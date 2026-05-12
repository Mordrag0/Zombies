// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZTabListWidget.h"
#include "UI/ZButton.h"

void UZTabListWidget::SetTabDisplayNames(const TMap<FName, FText>& InNames)
{
	TabDisplayNames = InNames;
}

void UZTabListWidget::HandleTabCreation_Implementation(FName TabNameID, UCommonButtonBase* TabButton)
{
	if (UZButton* Button = Cast<UZButton>(TabButton))
	{
		if (const FText* DisplayName = TabDisplayNames.Find(TabNameID))
		{
			Button->SetText(*DisplayName);
		}
	}
	TabButtonContainer->AddChild(TabButton);
}

