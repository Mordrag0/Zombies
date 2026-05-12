// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZRootLayoutWidget.generated.h"

class UZActivatableWidgetStack;
class UZNotificationManagerWidget;
class UZCompassWidget;
class AZFPCharacter;
enum class EZNotificationType : uint8;

enum class EZActivatableWidgetType : uint8
{
	Game,
	Menu,
	Popup
};

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZRootLayoutWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UZActivatableWidgetStack* GetWidgetStack(EZActivatableWidgetType WidgetType) const;

	void AddNotification(EZNotificationType NotificationType, const FText& Message);

	void Init(AZFPCharacter* FPCharacter);
	
protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UZActivatableWidgetStack> GameLayer;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UZActivatableWidgetStack> MenuLayer;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UZActivatableWidgetStack> PopupLayer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> Background;

	UPROPERTY()
	TArray<TObjectPtr<UZActivatableWidgetStack>> StacksWithBackground;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZNotificationManagerWidget> NotificationManager;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZCompassWidget> Compass;
};
