// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZActivatableWidgetStack.h"
#include "UI/ZActivatableWidget.h"

UZActivatableWidgetStack::UZActivatableWidgetStack(const FObjectInitializer& Initializer) : Super(Initializer)
{
	bHasBackground = true;
}

void UZActivatableWidgetStack::OnWidgetAddedToList(UCommonActivatableWidget& AddedWidget)
{
	Super::OnWidgetAddedToList(AddedWidget);

	if (UZActivatableWidget* ZWidget = Cast<UZActivatableWidget>(&AddedWidget))
	{
		ZWidget->SetOwningStack(this);
	}
}

