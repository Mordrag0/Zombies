// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "ZNotificationManagerWidget.generated.h"

class UZNotificationWidget;
class UVerticalBox;
class UTexture2D;
class UCommonTextStyle;
enum class EZNotificationType : uint8;

USTRUCT()
struct FZNotificationStyle
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UCommonTextStyle> TextStyle;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UTexture2D> Icon;
};

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZNotificationManagerWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UZNotificationManagerWidget(const FObjectInitializer& ObjectInitializer);

	void AddNotification(EZNotificationType NotificationType, const FText& Message);

protected:
	void OnNotificationExpired(UZNotificationWidget* WidgetToRemove);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZNotificationWidget> NotificationWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	float NotificationDuration;

	UPROPERTY(EditDefaultsOnly)
	FZNotificationStyle DefaultNotificationStyle;

	UPROPERTY(EditDefaultsOnly)
	TMap<EZNotificationType, FZNotificationStyle> NotificationStyles;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> NotificationBox;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxNotificationsAtOnce;

	TArray<TPair<EZNotificationType, FText>> PendingNotifications;
};

