// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZRootLayoutWidget.h"
#include "UI/ZActivatableWidgetStack.h"
#include "UI/ZCompassWidget.h"
#include "UI/ZNotificationManagerWidget.h"

UZActivatableWidgetStack* UZRootLayoutWidget::GetWidgetStack(EZActivatableWidgetType WidgetType) const
{
	return (WidgetType == EZActivatableWidgetType::Game) ? GameLayer : (WidgetType == EZActivatableWidgetType::Menu) ? MenuLayer : PopupLayer;
}

void UZRootLayoutWidget::AddNotification(EZNotificationType NotificationType, const FText& Message)
{
	NotificationManager->AddNotification(NotificationType, Message);
}

void UZRootLayoutWidget::Init(AZFPCharacter* FPCharacter)
{
	Compass->Init(FPCharacter);
}

void UZRootLayoutWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Background->SetVisibility(ESlateVisibility::Hidden);
	if (GameLayer->GetHasBackground())
	{
		StacksWithBackground.Add(GameLayer);
	}
	if (MenuLayer->GetHasBackground())
	{
		StacksWithBackground.Add(MenuLayer);
	}
	if (PopupLayer->GetHasBackground())
	{
		StacksWithBackground.Add(PopupLayer);
	}
	for (UZActivatableWidgetStack* Stack : StacksWithBackground)
	{
		Stack->OnDisplayedWidgetChanged().AddLambda([this](UCommonActivatableWidget* ActivatableWidget)
		{
			for (UZActivatableWidgetStack* Stack : StacksWithBackground)
			{
				if (Stack->GetActiveWidget())
				{
					Background->SetVisibility(ESlateVisibility::Visible);
					return;
				}
			}
			Background->SetVisibility(ESlateVisibility::Hidden);
		});
	}
}

