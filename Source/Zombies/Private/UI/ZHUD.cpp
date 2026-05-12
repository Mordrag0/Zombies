// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZHUD.h"
#include "UI/ZRootLayoutWidget.h"
#include "UI/ZHUDWidget.h"
#include "ZPlayerController.h"

void AZHUD::AddNotification(EZNotificationType NotificationType, const FText& Message)
{
	if (RootLayout)
	{
		RootLayout->AddNotification(NotificationType, Message);
	}
}

void AZHUD::Init(AZFPCharacter* FPCharacter)
{
	if (HUDWidget)
	{
		HUDWidget->Init(FPCharacter);
	}
	if (RootLayout)
	{
		RootLayout->Init(FPCharacter);
	}
}

void AZHUD::BeginPlay()
{
	Super::BeginPlay();

	if (RootLayoutClass)
	{
		RootLayout = CreateWidget<UZRootLayoutWidget>(GetOwningPlayerController(), RootLayoutClass);
		RootLayout->AddToViewport();
	}
	if (HUDWidgetClass)
	{
		HUDWidget = PushActivatableWidget<UZHUDWidget>(EZActivatableWidgetType::Game, HUDWidgetClass);
	}

	if (const AZPlayerController* PC = Cast<AZPlayerController>(GetOwningPlayerController()))
	{
		Init(PC->GetFPCharacter());
	}
}

