// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZMapWidget.h"
#include "ZGameState.h"
#include "CommonTextBlock.h"


void UZMapWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	if (AZGameState* GS = GetWorld()->GetGameState<AZGameState>())
	{
		BIND_MULTICAST_UOBJECT(OnUpdateTimeConnection, GS, GS->OnTimeChanged, this, &UZMapWidget::UpdateTime);
		UpdateTime(GS->GetTimeOfDay(), GS->GetDay());
	}
}

void UZMapWidget::NativeOnDeactivated()
{
	OnUpdateTimeConnection.Disconnect();

	Super::NativeOnDeactivated();
}

void UZMapWidget::UpdateTime(float InTimeOfDay, int32 InDay)
{
	int32 Hours = FMath::FloorToInt(InTimeOfDay);
	int32 Minutes = FMath::FloorToInt((InTimeOfDay - Hours) * 60.f);
	FNumberFormattingOptions FormatOptions;
	FormatOptions.MinimumIntegralDigits = 2;
	FormatOptions.MaximumIntegralDigits = 2;

	FText TimeText = FText::Format(INVTEXT("{0}:{1}"), FText::AsNumber(Hours, &FormatOptions), FText::AsNumber(Minutes, &FormatOptions));
	Time->SetText(TimeText);

	Day->SetText(FText::AsNumber(InDay));
}

