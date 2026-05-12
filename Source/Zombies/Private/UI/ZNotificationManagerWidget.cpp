// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZNotificationManagerWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/ZNotificationWidget.h"
#include "ZTypes.h"


UZNotificationManagerWidget::UZNotificationManagerWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NotificationDuration = 3.f;
	MaxNotificationsAtOnce = 5; // #ZTODO: test with 1
}

void UZNotificationManagerWidget::AddNotification(EZNotificationType NotificationType, const FText& Message)
{
	if (NotificationBox->GetAllChildren().Num() >= MaxNotificationsAtOnce)
	{
		PendingNotifications.Add(TPair<EZNotificationType, FText>(NotificationType, Message));
		return;
	}
	UZNotificationWidget* NotificationWidget = CreateWidget<UZNotificationWidget>(this, NotificationWidgetClass);
	FZNotificationStyle* Style = NotificationStyles.Find(NotificationType);
	if (!Style)
	{
		Style = &DefaultNotificationStyle;
	}
	NotificationWidget->Show(Message, Style);
	UVerticalBoxSlot* VBSlot = NotificationBox->AddChildToVerticalBox(NotificationWidget);
	VBSlot->SetHorizontalAlignment(HAlign_Center);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &UZNotificationManagerWidget::OnNotificationExpired, NotificationWidget), NotificationDuration, false);
}

void UZNotificationManagerWidget::OnNotificationExpired(UZNotificationWidget* WidgetToRemove)
{
	NotificationBox->RemoveChild(WidgetToRemove);
	if (PendingNotifications.Num() > 0)
	{
		const TPair<EZNotificationType, FText> PendingNotification = PendingNotifications[0];
		PendingNotifications.RemoveAt(0);
		AddNotification(PendingNotification.Key, PendingNotification.Value);
	}
}

